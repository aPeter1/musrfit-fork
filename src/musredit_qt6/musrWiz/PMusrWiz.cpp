/***************************************************************************

  PMusrWiz.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007-2021 by Andreas Suter                              *
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

#include <iostream>

#include <QtWidgets>
#include <QObject>
#include <QPixmap>
#include <QLabel>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QFrame>
#include <QRect>
#include <QIntValidator>
#include <QDate>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QInputDialog>

#include <QDebug>

#include "musrWiz.h"
#include "PTheoTemplate.h"
#include "PMusrfitFunc.h"
#include "PMusrWiz.h"

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * @brief PMsrData::PMsrData
 */
PMsrData::PMsrData()
{
  fTemplate = -1;

  fMsrFileName = QString("");
  fRunNumber = -1;
  fInstitute = "PSI";
  fInstrument = "HAL9500";
  fFitType = FIT_TYPE_SINGLE_HISTO;
  fTypeOfMeasurement = MEASURE_UNDEF;
  fT0Tag = T0_FROM_FILE;
  fT0 = -1;
  QDate date = QDate::currentDate();
  fYear = QString("%1").arg(date.year());

  fPacking = 1;
  fFitStart = 0.0;
  fFitEnd = 10.0;
  fCommands = "";

  fTheo = "";
}

//-------------------------------------------------------------------------
/**
 * @brief PMsrData::~PMsrData
 */
PMsrData::~PMsrData()
{

}

//-------------------------------------------------------------------------
/**
 * @brief PMsrData::getFitTypeString
 * @return
 */
QString PMsrData::getFitTypeString()
{
  QString str("UnDef");

  switch (fFitType) {
  case FIT_TYPE_ASYMMETRY:
    str = QString("Asymmetry");
    break;
  case FIT_TYPE_ASYMMETRY_RRF:
    str = QString("Asymmetry RRF");
    break;
  case FIT_TYPE_SINGLE_HISTO:
    str = QString("Single Histo");
    break;
  case FIT_TYPE_SINGLE_HISTO_RRF:
    str = QString("Single Histo RRF");
    break;
  case FIT_TYPE_MU_MINUS:
    str = QString("Mu Minus");
    break;
  case FIT_TYPE_NONE_MUSR:
    str = QString("None Musr");
    break;
  case FIT_TYPE_UNDEF:
  default:
    break;
  }

  return str;
}

//-------------------------------------------------------------------------
/**
 * @brief PMsrData::setParam
 * @param idx
 * @param param
 */
void PMsrData::setParam(int idx, PParam param)
{
  if (idx >= fParam.size())
    fParam.resize(idx+1);

  fParam[idx] = param;
}

//-------------------------------------------------------------------------
/**
 * @brief PMsrData::setMap
 * @param idx
 * @param map
 */
void PMsrData::setMap(int idx, PParam map)
{
  if (idx >= fMap.size())
    fMap.resize(idx+1);

  fMap[idx] = map;
}

//-------------------------------------------------------------------------
/**
 * @brief PMsrData::appendParam
 * @param param
 */
void PMsrData::appendParam(QVector<PParam> param)
{
  // check if parameter is aleady present. If yes, ignore it, otherwise add it
  bool present = false;
  for (int i=0; i<param.size(); i++) {
    present = false;
    for (int j=0; j<fParam.size(); j++) {
      if (param[i].getNumber() == fParam[j].getNumber()) { // only check parameter number, NOT name
        present = true;
      }
    }
    if (!present) { // add it
      fParam.push_back(param[i]);
    }
  }
}

//-------------------------------------------------------------------------
/**
 * @brief PMsrData::appendMap
 * @param map
 */
void PMsrData::appendMap(QVector<PParam> map)
{
  // check if map is aleady present. If yes, replace it, otherwise add it
  bool present = false;
  for (int i=0; i<map.size(); i++) {
    present = false;
    for (int j=0; j<fMap.size(); j++) {
      if (map[i].getNumber() == fMap[j].getNumber()) {
        present = true;
        fMap[j] = map[i];
      }
    }
    if (!present) { // add it
      fMap.push_back(map[i]);
    }
  }
}

//-------------------------------------------------------------------------
/**
 * @brief PMsrData::setMapTemplateName
 * @param idx
 * @param name
 */
void PMsrData::setMapTemplateName(int idx, QString name)
{
  if ((idx < 0) || (idx > fMap.size()))
    return;

  fMap[idx].setName(name);
}

//-------------------------------------------------------------------------
/**
 * @brief PMsrData::setFunc
 * @param funNo
 * @param str
 */
void PMsrData::setFunc(int funNo, QString str)
{
  if (funNo < 0)
    return;

  // check if not already present
  for (int i=0; i<fFunc.size(); i++) {
    if (fFunc[i].number == funNo) {
      if (str != "??")
        fFunc[i].fun = str;
      return;
    }
  }

  PFunc fun;
  fun.number = funNo;
  fun.fun = str;

  fFunc.append(fun);
}

//-------------------------------------------------------------------------
/**
 * @brief PMsrData::getParam
 * @param idx
 * @return
 */
PParam PMsrData::getParam(int idx)
{
  PParam param;

  if (idx > fParam.size())
    return param;

  return fParam[idx];
}

//-------------------------------------------------------------------------
/**
 * @brief PMsrData::getMap
 * @param idx
 * @return
 */
PParam PMsrData::getMap(int idx)
{
  PParam map;

  if (idx >= fMap.size())
    return map;

  return fMap[idx];
}

//-------------------------------------------------------------------------
/**
 * @brief PMsrData::getFuncNo
 * @param idx
 * @return
 */
int PMsrData::getFuncNo(int idx)
{
  if ((idx < 0) || (idx >= fFunc.size()))
    return -1;

  return fFunc[idx].number;
}

//-------------------------------------------------------------------------
/**
 * @brief PMsrData::isPresent
 * @param funStr
 * @return
 */
bool PMsrData::isPresent(const QString funStr)
{
  for (int i=0; i<fFunc.size(); i++) {
    if (fFunc[i].fun.startsWith(funStr, Qt::CaseInsensitive))
      return true;
  }

  return false;
}

//-------------------------------------------------------------------------
/**
 * @brief PMsrData::getFunc
 * @param funNo
 * @return
 */
QString PMsrData::getFunc(int funNo)
{
  int idx=-1;
  for (int i=0; i<fFunc.size(); i++) {
    if (fFunc[i].number == funNo) {
      idx=i;
      break;
    }
  }

  QString funStr = QString("fun%1 = ??").arg(funNo);

  if (idx != -1)
    funStr = fFunc[idx].fun;

  return funStr;
}

//-------------------------------------------------------------------------
/**
 * @brief PMsrData::getFuncAll
 * @return
 */
QString PMsrData::getFuncAll()
{
  QString funStr("");

  for (int i=0; i<fFunc.size(); i++) {
    funStr += fFunc[i].fun + "\n";
  }

  return funStr;
}

//-------------------------------------------------------------------------
/**
 * @brief PMsrData::sort
 * @param whichVec can be 'param', 'map', 'func'
 */
void PMsrData::sort(QString whichVec)
{
  bool swapped = true;
  int j=0;

  if (whichVec == "param") {
    PParam swap;
    while (swapped) {
      swapped = false;
      j++;
      for (int i=0; i < fParam.size() - j; i++) {
        if (fParam[i].getNumber() > fParam[i+1].getNumber()) {
          swap = fParam[i];
          fParam[i] = fParam[i+1];
          fParam[i+1] = swap;
          swapped = true;
        }
      }
    }
  } else if (whichVec == "map") {
    PParam swap;
    while (swapped) {
      swapped = false;
      j++;
      for (int i=0; i < fMap.size() - j; i++) {
        if (fMap[i].getNumber() > fMap[i+1].getNumber()) {
          swap = fMap[i];
          fMap[i] = fMap[i+1];
          fMap[i+1] = swap;
          swapped = true;
        }
      }
    }
  } else if (whichVec == "func") {
    PFunc swap;
    while (swapped) {
      swapped = false;
      j++;
      for (int i=0; i < fFunc.size() - j; i++) {
        if (fFunc[i].number > fFunc[i+1].number) {
          swap = fFunc[i];
          fFunc[i] = fFunc[i+1];
          fFunc[i+1] = swap;
          swapped = true;
        }
      }
    }
  }
}

//-------------------------------------------------------------------------
/**
 * @brief PMsrData::removeFunc
 * @param funList
 */
void PMsrData::removeFunc(QVector<int> &funList)
{
  int idx;
  for (int i=0; i<fFunc.size(); i++) {
    idx = -1;
    for (int j=0; j<funList.size(); j++) {
      if (fFunc[i].number == funList[j]) {
        idx = j;
        break;
      }
    }

    if (idx == -1) { // function not found hence remove it
      fFunc.remove(i);
    }
  }
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PShowTheo::PShowTheo(QString theo, QString func, QWidget *parent, Qt::WindowFlags f) :
  QDialog(parent, f)
{
  setWindowTitle("used Theory");

  QLabel *label = new QLabel("<h2>used Theory</h2>");
  QPlainTextEdit *theoTextEdit = new QPlainTextEdit(theo);
  theoTextEdit->setReadOnly(true);
  QPlainTextEdit *funcTextEdit = 0;
  if (!func.isEmpty()) {
    funcTextEdit = new QPlainTextEdit(func);
    funcTextEdit->setReadOnly(true);
  }
  QPushButton *cancel = new QPushButton("&Cancel");

  QVBoxLayout *layout = new QVBoxLayout;
  layout->addWidget(label);
  layout->addWidget(theoTextEdit);
  if (funcTextEdit)
    layout->addWidget(funcTextEdit);
  layout->addWidget(cancel);

  setLayout(layout);

  connect(cancel, SIGNAL(pressed()), this, SLOT(accept()));
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * @brief PIntroPage::PIntroPage
 * @param data
 * @param parent
 */
PIntroPage::PIntroPage(PAdmin *admin, PMsrData *data, QWidget *parent) :
  QWizardPage(parent),
  fAdmin(admin),
  fMsrData(data)
{
  setTitle("<h2>Introduction</h2>");
  setSubTitle("This wizard will help you to create your msr-file.");

  fMsrFileName = new QLineEdit();
  fMsrFileName->setToolTip("enter msr-file name or leave it empty.");
  fMsrFileName->setWhatsThis("If empty the file name will be generate,\nbased on the run number, the fit type\nand the type of measurement.");

  fYear = new QLineEdit();
  fYear->setValidator(new QIntValidator());
  QDate date = QDate::currentDate();
  fYear->setText(QString("%1").arg(date.year()));
  fYear->setToolTip("year when the run took place.");
  fYear->setWhatsThis("The year is used to generate\nthe RUN header information where\nmusrfit will look for the data.");

  fRunNumber = new QLineEdit();
  fRunNumber->setValidator(new QIntValidator());
  fRunNumber->setText(QString("%1").arg(fMsrData->getRunNumber()));
  fRunNumber->setToolTip("enter the run number here.");

  fInstitute = new QComboBox();
  QStringList list = fAdmin->getInstituteList(); // get list form the instrument_def's
  list.prepend("UnDef");
  fInstitute->addItems(list);
  int idx = fInstitute->findText(fAdmin->getDefaultInstitute());
  if (idx != -1)
    fInstitute->setCurrentIndex(idx);

  fInstrument = new QComboBox();
  list.clear();
  list << fAdmin->getInstrumentList(fMsrData->getInstitute());
  list.prepend("UnDef");
  fInstrument->addItems(list);
  idx = fInstrument->findText(fAdmin->getDefaultInstrument());
  if (idx != -1)
    fInstrument->setCurrentIndex(idx);

  fFitType = new QComboBox();
  list.clear();
  list << "UnDef" << "Single Histo" << "Single Histo RRF" << "Asymmetry" << "Asymmetry RRF" << "Mu Minus" << "None muSR";
  fFitType->addItems(list);
  idx = fFitType->findText(fAdmin->getDefaultFitType());
  fFitType->setCurrentIndex(idx);

  fMeasurementType = new QComboBox();
  list.clear();
  list << "UnDef" << "ZF" << "TF" << "LF";
  fMeasurementType->addItems(list);
  fMeasurementType->setCurrentIndex(fMsrData->getTypeOfMeasurement());

  fT0 = new QComboBox();
  list.clear();
  list << "from data file" << "call musrT0" << "enter here";
  fT0->addItems(list);
  fT0->setCurrentIndex(fMsrData->getT0Tag());
  fT0->setWhatsThis("choose if you want the t0's from the data file,\ncall musrT0 after the msr-files is generated,\nor enter the values within the wizard.");

  QFormLayout *layout = new QFormLayout;

  layout->addRow("Add msr-file name:", fMsrFileName);
  layout->addRow("Year:", fYear);
  layout->addRow("Run Number:", fRunNumber);
  layout->addRow("Institute:", fInstitute);
  layout->addRow("Instrument:", fInstrument);
  layout->addRow("Fit Type:", fFitType);
  layout->addRow("Type of Measurement:", fMeasurementType);
  layout->addRow("T0's:", fT0);

  setLayout(layout);

  registerField("runNumber*", fRunNumber);

  QObject::connect(fInstitute, SIGNAL(activated(int)), this, SLOT(handleInstituteChanged(int)));
  QObject::connect(fFitType, SIGNAL(activated(int)), this , SLOT(handleFitType(int)));
  QObject::connect(fMeasurementType, SIGNAL(activated(int)), this, SLOT(checkSetup(int)));  
  QObject::connect(fT0, SIGNAL(activated(int)), this, SLOT(handleT0(int)));
}

//-------------------------------------------------------------------------
void PIntroPage::handleInstituteChanged(int idx)
{
  QString str = fInstitute->itemText(idx);
  QStringList list = fAdmin->getInstrumentList(str);
  list.prepend("UnDef");

  fInstrument->clear();
  fInstrument->addItems(list);
}

//-------------------------------------------------------------------------
/**
 * @brief PIntroPage::handleFitType
 * @param idx
 */
void PIntroPage::handleFitType(int idx)
{
  if ( (idx != FIT_TYPE_SINGLE_HISTO) &&
       (idx != FIT_TYPE_ASYMMETRY) ){
    QMessageBox::warning(this, "WARNING", "Currently only fit type: single histo and asymmetry available.");
    fFitType->setCurrentIndex(FIT_TYPE_SINGLE_HISTO);
  }
}

//-------------------------------------------------------------------------
/**
 * @brief PIntroPage::checkSetup
 * @param idx
 */
void PIntroPage::checkSetup(int idx)
{
  QString measure("");
  PInstrument *instru = fAdmin->getInstrument(fInstitute->currentText(), fInstrument->currentText());
  QVector<PSetup> setup;
  switch (idx) {
  case MEASURE_ZF:
    setup = instru->getZFSetups();
    measure = "ZF";
    break;
  case MEASURE_TF:
    setup = instru->getTFSetups();
    measure = "TF";
    break;
  case MEASURE_LF:
    setup = instru->getLFSetups();
    measure = "LF";
    QMessageBox::information(this, "INFO", "Not yet implemented.");
    break;
  case MEASURE_UNDEF:
  default:
    break;
  }

  if (setup.size() == 0) {
    QString msg = QString("Didn't find any setup for:\nInstitute: %1\nInstrument: %2").arg(fInstitute->currentText()).arg(fInstrument->currentText());
    QMessageBox::critical(this, "ERROR", msg);
    return;
  } else if (setup.size() == 1) {
    if (setup[0].getName() == "Default") {
      fMsrData->setSetup("Default");
      return;
    }
  }

  QStringList items;
  for (int i=0; i<setup.size(); i++) {
    items << setup[i].getName();
  }

  bool ok;
  QString setupName = QInputDialog::getItem(this, "Choose Setup:", measure, items, 0, false, &ok);

  if (ok) {
    fMsrData->setSetup(setupName);
  }
}

//-------------------------------------------------------------------------
/**
 * @brief PIntroPage::handleT0
 * @param idx
 */
void PIntroPage::handleT0(int idx)
{
  bool ok;
  if (idx == T0_ENTER_WIZ) {
    int t0 = QInputDialog::getInt(this, "Enter T0:", "T0:", 0, 0, 2147483647, 1, &ok);
    if (ok)
      fMsrData->setT0(t0);
  }
}

//-------------------------------------------------------------------------
/**
 * @brief PIntroPage::nextId
 * @return
 */
int PIntroPage::nextId() const
{
  return PMusrWiz::ePageTheory;
}

//-------------------------------------------------------------------------
/**
 * @brief PIntroPage::validatePage
 * @return
 */
bool PIntroPage::validatePage()
{
  fMsrData->setMsrFileName(fMsrFileName->text());
  fMsrData->setYear(fYear->text());
  fMsrData->setRunNumber(fRunNumber->text().toInt());
  if (fInstitute->currentIndex() == INST_UNDEF)
    return false;
  fMsrData->setInstitute(fInstitute->currentText());
  if (fInstrument->currentIndex() == 0) // i.e. undefined
    return false;
  fMsrData->setInstrument(fInstrument->currentText());
  if (fFitType->currentIndex() == FIT_TYPE_UNDEF)
    return false;
  fMsrData->setFitType(fFitType->currentIndex());
  if (fMeasurementType->currentIndex() == MEASURE_UNDEF)
    return false;
  fMsrData->setTypeOfMeasurement(fMeasurementType->currentIndex());
  fMsrData->setT0Tag(fT0->currentIndex());

  return true;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * @brief PTheoPage::PTheoPage
 * @param data
 * @param parent
 */
PTheoPage::PTheoPage(PAdmin *admin, PMsrData *data, QWidget *parent) :
  QWizardPage(parent),
  fAdmin(admin),
  fMsrData(data)
{
  fTheoBlockNo = 0;

  fTheoValid = false;
  fHaveMap = false;
  fHaveFunc = false;

  setTitle("<h2>Theory</h2>");
  QString str = QString("Now you will need to enter your theory - fit type: %1").arg(fMsrData->getFitTypeString());
  setSubTitle(str);

  fTheo = new QPlainTextEdit();
  fTheo->setWhatsThis("Enter the theory function here.");

  fEditTemplate = new QCheckBox("Edit Template");
  fEditTemplate->setToolTip("if checked, it allows to edit the template");
  fEditTemplate->hide();

  fClearAll = new QPushButton();
  fClearAll->setText("Clear All");
  fClearAll->setToolTip("will clear the theory edit field.");
  fClearAll->setWhatsThis("will clear the theory edit field.");

  fCheckTheo = new QPushButton();
  fCheckTheo->setText("Check");
  fCheckTheo->setToolTip("check if the theory is valid.");
  fCheckTheo->setWhatsThis("Check is the theory string is valid.");

  QVector<PTheoTemplate> templ = fAdmin->getTheoTemplates();
  QVector<PMusrfitFunc> funcs = fAdmin->getMusrfitFunc();
  fTheoSelect = new QComboBox();
  QStringList list;
  // all the templates defined in musrfit_func.xml
  for (int i=0; i<templ.size(); i++) {
    list << templ[i].getName();
  }
  // all the functions defined in musrfit_func.xml
  for (int i=0; i<funcs.size(); i++) {
    list << funcs[i].getName();
  }
  fTheoSelect->addItems(list);
  fTheoSelect->setWhatsThis("select some predef. theory functions.\nFor all theory functions available check the help");

  fTheoAdd = new QPushButton("Add");

  QHBoxLayout *hLayout = new QHBoxLayout();
  hLayout->addWidget(fTheoSelect);
  hLayout->addWidget(fTheoAdd);

  QFormLayout *layout = new QFormLayout;
  layout->addRow("Enter Theory:", fTheo);
  layout->addRow(fEditTemplate);
  layout->addRow("Clear Theory:", fClearAll);
  layout->addRow("Pre Def. Theory Select:", hLayout);
  layout->addRow("Press for Validation:", fCheckTheo);

  setLayout(layout);

  QObject::connect(fEditTemplate, SIGNAL(stateChanged(int)), this, SLOT(templateState(int)));
  QObject::connect(fClearAll, SIGNAL(clicked()), this, SLOT(clearAll()));
  QObject::connect(fCheckTheo, SIGNAL(clicked()), this, SLOT(checkTheory()));
  QObject::connect(fTheoAdd, SIGNAL(clicked()), this, SLOT(addTheory()));
}

//-------------------------------------------------------------------------
void PTheoPage::initializePage()
{
  QString str = QString("Now you will need to enter your theory. Fit type: %1").arg(fMsrData->getFitTypeString());
  setSubTitle(str);
}

//-------------------------------------------------------------------------
/**
 * @brief PTheoPage::nextId
 * @return
 */
int PTheoPage::nextId() const
{
  if (fHaveFunc)
    return PMusrWiz::ePageFunctions;
  else if (fHaveMap)
    return PMusrWiz::ePageMaps;
  else
    return PMusrWiz::ePageParameters;
}

//-------------------------------------------------------------------------
/**
 * @brief PTheoPage::validatePage
 * @return
 */
bool PTheoPage::validatePage()
{
  if (fTheo->toPlainText().isEmpty())
    return false;

  fMsrData->clearParam();
  fFunList.clear();
//  fMsrData->clearFunc();
  fMsrData->clearMap();

  checkTheory();
  if (!fTheoValid)
    return false;

  // transfer theory string
  fMsrData->setTheory(fTheo->toPlainText());

  // transfer found parameters
  fMsrData->setParam(fParamList);

  // transfer found maps
  fMsrData->setMap(fMapList);

  // transfer found functions
  if (fHaveFunc) {
    QString funStr;
    for (int i=0; i<fFunList.size(); i++) {
      funStr = QString("fun%1").arg(fFunList[i]);
      if (!fMsrData->isPresent(funStr)) {
        funStr = QString("fun%1 = ??").arg(fFunList[i]);
        fMsrData->setFunc(fFunList[i], funStr);
      }
    }
  }

  // check if it is necessary to eliminate functions which are not present anymore
  if (fFunList.size() != fMsrData->getNoOfFunc()) {
    // remove functions from fMsrData which are NOT found in fFuncList
    fMsrData->removeFunc(fFunList);
  }

  return true;
}

//-------------------------------------------------------------------------
/**
 *
 */
void PTheoPage::templateState(int state)
{
  if (state == Qt::Checked) {
    fTheo->setTextInteractionFlags(Qt::TextEditorInteraction);
    fMsrData->setTemplate(-1);
    fEditTemplate->hide();
    fEditTemplate->setCheckState(Qt::Unchecked);
  }
}

//-------------------------------------------------------------------------
/**
 *
 */
void PTheoPage::clearAll()
{
  fTheo->clear();
  fTheo->setTextInteractionFlags(Qt::TextEditorInteraction);
  fEditTemplate->hide();
  fMsrData->setTemplate(-1);
}

//-------------------------------------------------------------------------
/**
 * @brief PTheoPage::addTheory
 */
void PTheoPage::addTheory()
{
  int idx = fTheoSelect->currentIndex();
  bool isTempl = false;
  QString item = fTheoSelect->currentText();
  if (item.startsWith("T:")) {
    fEditTemplate->show();
    isTempl = true;
  }

  QString theo("");
  bool emptyTheo = false;
  if (fTheo->toPlainText().isEmpty()) { // empty theory
    emptyTheo = true;
    theo = getTheoryFunction(idx);
    if (theo == "??")
      return;
    fTheo->setPlainText(theo);
  } else { // theory already has some stuff
    emptyTheo = false;
    theo = getTheoryFunction(idx);
    if (theo == "??")
      return;
    fTheo->appendPlainText("+");
    fTheo->appendPlainText(theo);
  }

  if (isTempl && emptyTheo) {
    fTheo->setTextInteractionFlags(Qt::TextBrowserInteraction);
    fMsrData->setTemplate(idx);
    fMsrData->setTemplate(fAdmin->getTheoTemplate(idx));
  }
}

//-------------------------------------------------------------------------
/**
 * @brief PTheoPage::checkTheory
 */
void PTheoPage::checkTheory()
{
  fTheoValid = false;

  // reset all lists:
  fParamList.clear();
  fMapList.clear();
  fFunList.clear();

  // check theory the following way:
  // (i) make sure that line by line is a valid function
  // (ii) make a collection of parameters with names suggested
  // (iii) make a collection of functions if present
  // (iv) make a map list if present

  QString str = fTheo->toPlainText();
  QStringList line = str.split("\n");
  bool ok = true;
  PMusrfitFunc func;
  for (int i=0; i<line.size(); i++) {
    if (line[i].trimmed().isEmpty() ||
        line[i].trimmed().startsWith("%") ||
        line[i].trimmed().startsWith("#")) { // empty of comment lines
      continue;
    } else if (line[i].trimmed().startsWith("+")) {
      fTheoBlockNo++;
      continue;
    } else if (line[i].startsWith("fun")) {
      ok = analyzeTokens(line[i], 0);
      if (!ok) {
        QString str = QString("**ERROR** in line %1.\n funX takes no parameter.").arg(i+1);
        QMessageBox::critical(this, "Check Theory", str);
        // eventually it would be nice to highlight the faulty line
        return;
      }
    } else if (line[i].startsWith("map")) {
        ok = analyzeTokens(line[i], 0);
        if (!ok) {
          QString str = QString("**ERROR** in line %1.\n mapX takes no parameter.").arg(i+1);
          QMessageBox::critical(this, "Check Theory", str);
          // eventually it would be nice to highlight the faulty line
          return;
        }
    } else { // assume musrfit functions here
      QStringList strList = line[i].split(" ", Qt::SkipEmptyParts);
      func = fAdmin->getMusrfitFunc(strList[0]);
      if (func.getName() == "UnDef") { // function not found
        QString str = QString("**ERROR** in line %1, '%2' is not a recognized musrfit function.").arg(i+1).arg(line[i]);
        QMessageBox::critical(this, "Check Theory", str);
        // eventually it would be nice to highlight the faulty line
        return;
      }
      ok = analyzeTokens(line[i], func.getNoOfParam());
      if (!ok) {
        QString str = QString("**ERROR** in line %1.\n %2 takes %3 parameter.").arg(i+1).arg(func.getName()).arg(func.getNoOfParam());
        QMessageBox::critical(this, "Check Theory", str);
        // eventually it would be nice to highlight the faulty line
        return;
      }
    }
  }

  if (fMapList.size() > 0)
    fHaveMap = true;
  else
    fHaveMap = false;

  if (fFunList.size() > 0)
    fHaveFunc = true;
  else
    fHaveFunc = false;

  fTheoValid = true;

  QObject *obj = sender();
  if (obj == fCheckTheo)
    QMessageBox::information(this, "Check Theory", "Theory seems to be OK.");
}

//-------------------------------------------------------------------------
/**
 * @brief PTheoPage::getTheoryFunction
 * @param idx
 * @return
 */
QString PTheoPage::getTheoryFunction(int idx)
{
  QString result("??");
  QVector<PTheoTemplate> templ = fAdmin->getTheoTemplates();
  QVector<PMusrfitFunc> func = fAdmin->getMusrfitFunc();
  static int mapCount = 1;

  if (idx >= templ.size()) { // deal with musrfit functions
    if (idx >= templ.size() + func.size())
      return result;
    result = func[idx-templ.size()].getAbbrv();
    for (int i=0; i<func[idx-templ.size()].getNoOfParam(); i++) {
      if (func[idx-templ.size()].getFuncParam(i).isMap()) {
        result += " " + QString("map%1").arg(mapCount);
        mapCount++;
      } else {
        result += " " + QString("%1").arg(i+1);
      }
    }
  } else { // deal with templates
    result = templ[idx].getTheory();
    result.replace("\\n", QString(QChar::LineSeparator));
    mapCount += templ[idx].getNoOfMap();
  }

  return result;
}

//-------------------------------------------------------------------------
/**
 * @brief PTheoPage::analyzeTokens
 * @param str
 * @param noOfTokens
 * @return
 */
bool PTheoPage::analyzeTokens(QString str, int noOfTokens)
{
  QStringList tok = str.trimmed().split(" ", Qt::SkipEmptyParts);

  bool ok;
  // check if line is of the form 'funX' or 'mapX'
  if (tok.size() == 1) {
    QString stok;
    int stokNo;
    if (tok[0].startsWith("map")) {
      stok = tok[0];
      stok.remove("map");
      stokNo = stok.toInt(&ok);
      if (ok) {
        dealWithMap(tok[0], stokNo, 0);
      } else {
        QString msg = QString("Found map of the form '%1'.\nThis is NOT a legal map!").arg(tok[0]);
        QMessageBox::critical(this, "analyzeTokens", msg);
        return false;
      }
    } else if (tok[0].startsWith("fun")) {
      stok = tok[0];
      stok.remove("fun");
      stokNo = stok.toInt(&ok);
      if (ok) {
        dealWithFun(stokNo);
      } else {
        QString msg = QString("Found fun of the form '%1'.\nThis is NOT a legal function!").arg(tok[0]);
        QMessageBox::critical(this, "analyzeTokens", msg);
        return false;
      }
    }
  }

  // check that the number of tokens is ok
  if (tok.size()-1 != noOfTokens) {
    return false;
  }

  // check if the token is a parameter
  int paramNo;
  for (int i=1; i<tok.size(); i++) {
    paramNo = tok[i].toInt(&ok);
    if (ok) { // i.e. it is a parameter
      dealWithParam(tok[0], paramNo, i);
    } else if (!tok[i].startsWith("map") && !tok[i].startsWith("fun")) {
      QString msg = QString("Found parameter of the form '%1'.\nThis is NOT a legal parameter!").arg(tok[i]);
      QMessageBox::critical(this, "analyzeTokens", msg);
      return false;
    }
  }

  // check if the token is a map
  QString map;
  int mapNo;
  for (int i=1; i<tok.size(); i++) {
    if (tok[i].startsWith("map")) {
      map = tok[i];
      map.remove("map");
      mapNo = map.toInt(&ok);
      if (ok) {
        dealWithMap(tok[0], mapNo, i);
      } else {
        QString msg = QString("Found map of the form '%1'.\nThis is NOT a legal map!").arg(tok[i]);
        QMessageBox::critical(this, "analyzeTokens", msg);
        return false;
      }
    }
  }

  // check if the token is a function
  QString fun;
  int funNo;
  for (int i=1; i<tok.size(); i++) {
    if (tok[i].startsWith("fun")) {
      fun = tok[i];
      fun.remove("fun");
      funNo = fun.toInt(&ok);
      if (ok) {
        dealWithFun(funNo);
      } else {
        QString msg = QString("Found function of the form '%1'.\nThis is NOT a legal function!").arg(tok[i]);
        QMessageBox::critical(this, "analyzeTokens", msg);
        return false;
      }
    }
  }

  return true;
}

//-------------------------------------------------------------------------
/**
 * @brief PTheoPage::dealWithParam
 * @param theo
 * @param paramNo
 * @param paramIdx
 */
void PTheoPage::dealWithParam(QString theo, int paramNo, int paramIdx)
{
  // check if paramNo is already in the parameter list
  for (int i=0; i<fParamList.size(); i++) {
    if (fParamList[i].getNumber() == paramNo) { // nothing to be done
      return;
    }
  }

  PParam par;
  PMusrfitFunc func = fAdmin->getMusrfitFunc(theo);
  if (func.getName() == "UnDef") {
    // something is fundamentaly wrong
    return;
  }

  QString str;
  par.setNumber(paramNo);
  str = QString("%1_%2").arg(func.getFuncParam(paramIdx-1).getParamName()).arg(fTheoBlockNo);
  par.setName(str);
  par.setValue(func.getFuncParam(paramIdx-1).getParamValue());
  if (par.getName().contains("Ph_"))
    par.setStep(10.0);
  else
    par.setStep(par.getValue() * 0.1);

  // keep parameter entry
  fParamList.push_back(par);
}

//-------------------------------------------------------------------------
/**
 * @brief PTheoPage::dealWithMap
 * @param theo
 * @param mapNo
 * @param paramIdx
 */
void PTheoPage::dealWithMap(QString theo, int mapNo, int paramIdx)
{
  // check if mapNo is already in the map list
  for (int i=0; i<fMapList.size(); i++) {
    if (fMapList[i].getNumber() == mapNo) {
      return;
    }
  }

  PParam map;
  QString str;

  if (theo.startsWith("map")) {
    map.setNumber(mapNo);
    str = QString("map%1").arg(mapNo);
    map.setName(str);
    fMapList.push_back(map);
  } else { // theory
    PMusrfitFunc func = fAdmin->getMusrfitFunc(theo);
    if (func.getName() == "UnDef") {
      // something is fundamentaly wrong
      return;
    }

    map.setNumber(mapNo);
    str = QString("%1_%2").arg(func.getFuncParam(paramIdx-1).getParamName()).arg(mapNo);
    map.setName(str);
    fMapList.push_back(map);
  }
}

//-------------------------------------------------------------------------
/**
 * @brief PTheoPage::dealWithFun
 * @param funNo
 */
void PTheoPage::dealWithFun(int funNo)
{
  // check if funNo is already in the fun list
  for (int i=0; i<fFunList.size(); i++) {
    if (fFunList[i] == funNo) {
      return;
    }
  }

  fFunList.push_back(funNo);
}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * @brief PMapPage::PMapPage
 */
PMapPage::PMapPage(PMsrData *data, QWidget *parent) :
  QWizardPage(parent),
  fMsrData(data)
{
  fMapPageLayout = 0;
}

//-------------------------------------------------------------------------
/**
 * @brief PMapPage::nextId
 * @return
 */
int PMapPage::nextId() const
{
  return PMusrWiz::ePageParameters;
}

//-------------------------------------------------------------------------
/**
 * @brief PMapPage::initializePage
 */
void PMapPage::initializePage()
{
  setTitle("<h2>Maps</h2>");
  QString str = QString("Here you can fine tune your map template names. Fit type: %1").arg(fMsrData->getFitTypeString());
  setSubTitle(str);

  // sort maps
  fMsrData->sort("map");

  fMapPageLayout = new QFormLayout;
  setLayout(fMapPageLayout);

  // make sure that the map list is empty
  fMapGuiVec.clear();

  // fill Map
  PParamGui mapGui;
  PParam map;

  // check if this is a template. If this is the case, fill fMsrData maps with the template info.
  if (fMsrData->isTemplate()) {
    PTheoTemplate templ = fMsrData->getTemplate();
    fMsrData->clearMap();
    for (int i=0; i<templ.getNoOfMap(); i++) {
      fMsrData->setMap(i, templ.getMap(i));
    }
  }

  for (int i=0; i<fMsrData->getNoOfMap(); i++) {
    map = fMsrData->getMap(i);
    mapGui.paramName = new QLineEdit(map.getName());
    mapGui.paramVal = new QLineEdit("3.45");
    mapGui.paramStep = new QLineEdit("0.01");
    mapGui.paramBoundLow = new QLineEdit();
    mapGui.paramBoundHigh = new QLineEdit();
    fMapGuiVec.push_back(mapGui);
  }

  QHBoxLayout *hLayout;

  // header
  hLayout = new QHBoxLayout();
  mapGui.paramName = new QLineEdit("Name");
  mapGui.paramName->setReadOnly(true);
  mapGui.paramVal = new QLineEdit("Value");
  mapGui.paramVal->setReadOnly(true);
  mapGui.paramStep = new QLineEdit("Step");
  mapGui.paramStep->setReadOnly(true);
  mapGui.paramBoundLow = new QLineEdit("BoundLow");
  mapGui.paramBoundLow->setReadOnly(true);
  mapGui.paramBoundHigh = new QLineEdit("BoundHigh");
  mapGui.paramBoundHigh->setReadOnly(true);
  hLayout->addWidget(mapGui.paramName);
  hLayout->addWidget(mapGui.paramVal);
  hLayout->addWidget(mapGui.paramStep);
  hLayout->addWidget(mapGui.paramBoundLow);
  hLayout->addWidget(mapGui.paramBoundHigh);

  fMapPageLayout->addRow("No", hLayout);

  for (int i=0; i<fMapGuiVec.size(); i++) {
    hLayout = new QHBoxLayout();
    hLayout->addWidget(fMapGuiVec[i].paramName);
    hLayout->addWidget(fMapGuiVec[i].paramVal);
    hLayout->addWidget(fMapGuiVec[i].paramStep);
    hLayout->addWidget(fMapGuiVec[i].paramBoundLow);
    hLayout->addWidget(fMapGuiVec[i].paramBoundHigh);

    fMapPageLayout->addRow(QString("map%1").arg(i+1), hLayout);
  }

  fShowTheo = new QPushButton("Show &Theory");
  fMapPageLayout->addRow(fShowTheo);

  update();

  connect(fShowTheo, SIGNAL(pressed()), this, SLOT(showTheo()));
}

//-------------------------------------------------------------------------
/**
 * @brief PMapPage::validatePage
 * @return
 */
bool PMapPage::validatePage()
{
  // collect the map information
  PParam map;
  QString str;
  double dval;
  bool ok;

  for (int i=0; i<fMapGuiVec.size(); i++) {
    map.setNumber(fMsrData->getMap(i).getNumber());
    str = fMapGuiVec[i].paramName->text();
    map.setName(str);

    dval = fMapGuiVec[i].paramVal->text().toDouble(&ok);
    if (ok)
      map.setValue(dval);
    else
      map.setValue(0.0);

    dval = fMapGuiVec[i].paramStep->text().toDouble(&ok);
    if (ok)
      map.setStep(dval);
    else
      map.setStep(0.0);

    map.setPosErr("none");
    map.setBoundLow(fMapGuiVec[i].paramBoundLow->text());
    map.setBoundHigh(fMapGuiVec[i].paramBoundHigh->text());

    fMsrData->setMap(i, map);
  }

  return true;
}

//-------------------------------------------------------------------------
/**
 * @brief PMapPage::showTheo
 */
void PMapPage::showTheo()
{
  PShowTheo *showTheo = new PShowTheo(fMsrData->getTheory(), fMsrData->getFuncAll(), this);
  showTheo->show();
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * @brief PFuncPage::PFuncPage
 */
PFuncPage::PFuncPage(PMsrData *data, QWidget *parent) :
  QWizardPage(parent),
  fMsrData(data)
{
  setTitle("<h2>Functions</h2>");
  QString str = QString("Now you will need to enter your functions. Fit type: %1").arg(fMsrData->getFitTypeString());
  setSubTitle(str);

  fFunc = new QPlainTextEdit();
  fFunc->setWhatsThis("Enter the function(s) here.");

  fShowTheo = new QPushButton("Show &Theory");

  QFormLayout *layout = new QFormLayout;
  layout->addRow("Enter Function(s):", fFunc);
  layout->addRow(fShowTheo);

  setLayout(layout);

  connect(fShowTheo, SIGNAL(pressed()), this, SLOT(showTheo()));
}

//-------------------------------------------------------------------------
/**
 * @brief PFuncPage::showTheo
 */
void PFuncPage::showTheo()
{
  PShowTheo *showTheo = new PShowTheo(fMsrData->getTheory(), "", this);
  showTheo->show();
}

//-------------------------------------------------------------------------
/**
 * @brief PFuncPage::nextId
 * @return
 */
int PFuncPage::nextId() const
{
  if (fMsrData->getNoOfMap() > 0)
    return PMusrWiz::ePageMaps;
  else
    return PMusrWiz::ePageParameters;
}

//-------------------------------------------------------------------------
/**
 *
 */
void PFuncPage::initializePage()
{
  QString str = QString("Now you will need to enter your functions. Fit type: %1").arg(fMsrData->getFitTypeString());
  setSubTitle(str);

  // check if this is a template. If this is the case, fill fMsrData functions with the template info.
  if (fMsrData->isTemplate()) {
    // fill functions according to the template
    PTheoTemplate templ = fMsrData->getTemplate();
    fMsrData->clearFunc();
    for (int i=0; i<templ.getNoOfFunc(); i++) {
      fMsrData->setFunc(templ.getFuncNo(i), templ.getFunc(i));
    }
  }

  fMsrData->sort("func");
  fFunc->clear();
  for (int i=0; i<fMsrData->getNoOfFunc(); i++) {
    fFunc->appendPlainText(fMsrData->getFunc(fMsrData->getFuncNo(i)));
  }
}

//-------------------------------------------------------------------------
/**
 *
 */
void PFuncPage::cleanupPage()
{
  fMsrData->clearFunc();

  // keep the function information
  QString str = fFunc->toPlainText();
  QStringList funList = str.split("\n");
  int funNo=0;
  for (int i=0; i<funList.size(); i++) {
    funNo = getFuncNo(funList[i]);
    if (funNo != -1)
      fMsrData->setFunc(funNo, funList[i]);
  }
}

//-------------------------------------------------------------------------
/**
 * @brief PFuncPage::validatePage
 * @return
 */
bool PFuncPage::validatePage()
{
  QVector<PParam> paramList;
  QVector<PParam> mapList;

  QString str = fFunc->toPlainText();
  QString sstr;
  QString tok;
  int idx = 0, parNo, mapNo;
  bool ok;

  // collect potentially present parameters
  PParam param;
  while ((idx = str.indexOf("par", idx)) != -1) {
    tok = "";
    idx += 3;
    while (str[idx].isDigit()) {
      tok += str[idx++];
    }
    parNo = tok.toInt(&ok);
    if (ok) {
      param.setNumber(parNo);
      sstr = QString("par%1").arg(parNo);
      param.setName(sstr);
      param.setValue(0.0);
      param.setStep(0.1);
      paramList.push_back(param);
    }
  }
  fMsrData->appendParam(paramList);

  // collect potentially present maps
  str = fFunc->toPlainText();
  PParam map;
  idx = 0;
  while ((idx = str.indexOf("map", idx)) != -1) {
    tok = "";
    idx += 3;
    while (str[idx].isDigit()) {
      tok += str[idx++];
    }
    mapNo = tok.toInt(&ok);
    if (ok) {
      map.setNumber(mapNo);
      sstr = QString("map%1").arg(mapNo);
      map.setName(sstr);
      mapList.push_back(map);
    }
  }
  fMsrData->appendMap(mapList);

  // keep the function information
  str = fFunc->toPlainText();
  QStringList funList = str.split("\n");
  int funNo = 0;
  fMsrData->clearFunc();
  for (int i=0; i<funList.size(); i++) {
    if (funList[i].isEmpty())
      continue;
    funNo = getFuncNo(funList[i]);
    if (funNo != -1)
      fMsrData->setFunc(i, funList[i]);
  }

  return true;
}

//-------------------------------------------------------------------------
/**
 * @brief PFuncPage::getFuncNo
 * @param str
 * @return
 */
int PFuncPage::getFuncNo(const QString str)
{
  QString strNo = str;

  // str should have to form fun<no> = <something>, where <no> is an int
  if (!str.startsWith("fun"))
    return -1;

  strNo.remove(0, 3); // get rid of 'fun'
  int idx = strNo.indexOf("=");
  if (idx == -1)
    return -1;

  strNo.remove(idx, str.length()-idx);

  bool ok;
  int ival = strNo.toInt(&ok);
  if (!ok)
    return -1;

  return ival;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * @brief PParamPage::PParamPage
 */
PParamPage::PParamPage(PMsrData *data, QWidget *parent) :
  QWizardPage(parent),
  fMsrData(data)
{
  fParameterPageLayout = 0;
}

//-------------------------------------------------------------------------
/**
 * @brief PParamPage::nextId
 * @return
 */
int PParamPage::nextId() const
{
  return PMusrWiz::ePageFitInfo;
}

//-------------------------------------------------------------------------
/**
 * @brief PParamPage::initializePage
 */
void PParamPage::initializePage()
{
  // sort parameter vector
  fMsrData->sort("param");

  setTitle("<h2>Fit Parameters</h2>");
  QString str = QString("Here you can fine tune your parameters. Fit type: %1").arg(fMsrData->getFitTypeString());
  setSubTitle(str);

  fParameterPageLayout = new QFormLayout;
  setLayout(fParameterPageLayout);

  // make sure that parameter list is empty
  fParamGuiVec.clear();

  // fill parameters
  PParamGui paramGui;
  PParam param;

  // if a template replace the parameters found so far by the template parameter settings
  if (fMsrData->isTemplate()) {
    fMsrData->clearParam();
    PTheoTemplate templ = fMsrData->getTemplate();
    for (int i=0; i<templ.getNoOfParam(); i++) {
      fMsrData->setParam(i, templ.getParam(i));
    }
  }

  for (int i=0; i<fMsrData->getNoOfParam(); i++) {
    param = fMsrData->getParam(i);
    paramGui.paramName = new QLineEdit(param.getName());
    paramGui.paramVal = new QLineEdit(QString("%1").arg(param.getValue()));
    paramGui.paramStep = new QLineEdit(QString("%1").arg(param.getStep()));
    paramGui.paramBoundLow = new QLineEdit();
    paramGui.paramBoundHigh = new QLineEdit();
    fParamGuiVec.push_back(paramGui);
  }

  QHBoxLayout *hLayout;

  // header
  hLayout = new QHBoxLayout();
  paramGui.paramName = new QLineEdit("Name");
  paramGui.paramName->setReadOnly(true);
  paramGui.paramVal = new QLineEdit("Value");
  paramGui.paramVal->setReadOnly(true);
  paramGui.paramStep = new QLineEdit("Step");
  paramGui.paramStep->setReadOnly(true);
  paramGui.paramBoundLow = new QLineEdit("BoundLow");
  paramGui.paramBoundLow->setReadOnly(true);
  paramGui.paramBoundHigh = new QLineEdit("BoundHigh");
  paramGui.paramBoundHigh->setReadOnly(true);
  hLayout->addWidget(paramGui.paramName);
  hLayout->addWidget(paramGui.paramVal);
  hLayout->addWidget(paramGui.paramStep);
  hLayout->addWidget(paramGui.paramBoundLow);
  hLayout->addWidget(paramGui.paramBoundHigh);
  fParameterPageLayout->addRow("No", hLayout);

  for (int i=0; i<fParamGuiVec.size(); i++) {
    hLayout = new QHBoxLayout();
    hLayout->addWidget(fParamGuiVec[i].paramName);
    hLayout->addWidget(fParamGuiVec[i].paramVal);
    hLayout->addWidget(fParamGuiVec[i].paramStep);
    hLayout->addWidget(fParamGuiVec[i].paramBoundLow);
    hLayout->addWidget(fParamGuiVec[i].paramBoundHigh);
    fParameterPageLayout->addRow(QString("%1").arg(i+1), hLayout);
  }

  fShowTheo = new QPushButton("Show &Theory");
  fParameterPageLayout->addRow(fShowTheo);

  update();

  connect(fShowTheo, SIGNAL(pressed()), this, SLOT(showTheo()));
}

//-------------------------------------------------------------------------
/**
 * @brief PParamPage::validatePage
 * @return
 */
bool PParamPage::validatePage()
{
  PParam param;
  QString str;
  double dval;
  bool ok;

  for (int i=0; i<fParamGuiVec.size(); i++) {
    param.setNumber(fMsrData->getParam(i).getNumber());
    str = fParamGuiVec[i].paramName->text();
    param.setName(str);

    dval = fParamGuiVec[i].paramVal->text().toDouble(&ok);
    if (ok)
      param.setValue(dval);
    else
      param.setValue(0.0);

    dval = fParamGuiVec[i].paramStep->text().toDouble(&ok);
    if (ok)
      param.setStep(dval);
    else
      param.setStep(0.0);

    param.setPosErr("none");
    param.setBoundLow(fParamGuiVec[i].paramBoundLow->text());
    param.setBoundHigh(fParamGuiVec[i].paramBoundHigh->text());

    fMsrData->setParam(i, param);
  }

  return true;
}

//-------------------------------------------------------------------------
/**
 * @brief PParamPage::showTheo
 */
void PParamPage::showTheo()
{
  PShowTheo *showTheo = new PShowTheo(fMsrData->getTheory(), fMsrData->getFuncAll(), this);
  showTheo->show();
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * @brief PFitInfoPage::PFitInfoPage
 */
PFitInfoPage::PFitInfoPage(PMsrData *data, QWidget *parent) :
  QWizardPage(parent),
  fMsrData(data)
{
  setTitle("<h2>Fit Info</h2>");
  QString str = QString("Collect necessary fit information. Fit type: %1").arg(fMsrData->getFitTypeString());
  setSubTitle(str);

  fFitRangeStart = new QLineEdit("0.0");
  fFitRangeStart->setValidator(new QDoubleValidator());
  fFitRangeEnd = new QLineEdit("9.0");
  fFitRangeEnd->setValidator(new QDoubleValidator());

  QHBoxLayout *hLayout = new QHBoxLayout();
  hLayout->addWidget(fFitRangeStart);
  hLayout->addWidget(fFitRangeEnd);

  fPacking = new QLineEdit("1");
  fPacking->setValidator(new QIntValidator());

  fCommands = new QPlainTextEdit();
  fCommands->setPlainText("#MAX_LIKELIHOOD\nPRINT_LEVEL 2\nMINIMIZE\nMINOS\nSAVE");

  QFormLayout *layout = new QFormLayout;
  layout->addRow("Fit Range:", hLayout);
  layout->addRow("Packing:", fPacking);
  layout->addRow("Commands:", fCommands);

  setLayout(layout);
}

//-------------------------------------------------------------------------
/**
 * @brief PFitInfoPage::initializePage
 */
void PFitInfoPage::initializePage()
{
  QString str = QString("Collect necessary fit information. Fit type: %1").arg(fMsrData->getFitTypeString());
  setSubTitle(str);
}

//-------------------------------------------------------------------------
/**
 * @brief PFitInfoPage::nextId
 * @return
 */
int PFitInfoPage::nextId() const
{
  return PMusrWiz::ePageConclusion;
}

//-------------------------------------------------------------------------
/**
 * @brief PFitInfoPage::validatePage
 * @return
 */
bool PFitInfoPage::validatePage()
{
  bool ok;
  int ival = fPacking->text().toInt(&ok);
  if (ok)
    fMsrData->setPacking(ival);

  double dval = fFitRangeStart->text().toDouble(&ok);
  if (ok)
    fMsrData->setFitStart(dval);
  dval = fFitRangeEnd->text().toDouble(&ok);
  if (ok)
    fMsrData->setFitEnd(dval);

  fMsrData->setCmd(fCommands->toPlainText());

  return true;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * @brief PConclusionPage::PConclusionPage
 */
PConclusionPage::PConclusionPage(PAdmin *admin, PMsrData *data, QString *msrFilePath, QWidget *parent) :
  QWizardPage(parent),
  fMsrFilePath(msrFilePath),
  fAdmin(admin),
  fMsrData(data)
{
  QDir ddir = QDir(*fMsrFilePath);

  setTitle("<h2>Create</h2>");
  setSubTitle("Now we create the msr-file.");

  QVBoxLayout *vLayout = new QVBoxLayout;

  fMsrPathFileLabel = new QLabel("Current msr-File Path:");
  fMsrFilePathLineEdit = new QLineEdit(ddir.absolutePath());
  fMsrFilePathLineEdit->setReadOnly(true);
  fSaveAsMsrFile = new QPushButton("Save As (msr-file path)");
  fSaveAsTemplate = new QPushButton("Save As (template)");

  vLayout->addWidget(fMsrPathFileLabel);
  vLayout->addWidget(fMsrFilePathLineEdit);
  vLayout->addWidget(fSaveAsMsrFile);
  vLayout->addStretch(1);
  vLayout->addWidget(fSaveAsTemplate);

  setLayout(vLayout);

  connect(fSaveAsMsrFile, SIGNAL(pressed()), this, SLOT(saveAsMsrFile()));
  connect(fSaveAsTemplate, SIGNAL(pressed()), this, SLOT(saveAsTemplate()));
}

//-------------------------------------------------------------------------
void PConclusionPage::saveAsMsrFile()
{
  QString str = QFileDialog::getExistingDirectory(this, tr("Save in Directory"), "./",
                                                  QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
  if (!str.isEmpty()) {
    *fMsrFilePath = str;
    fMsrFilePathLineEdit->setText(str);
  }
}

//-------------------------------------------------------------------------
/**
 *
 */
void PConclusionPage::saveAsTemplate()
{
  bool newTemplate = false;

  QStringList list;
  list << "<new>";
  for (int i=0; i<fAdmin->getTheoTemplateSize(); i++)
    list << fAdmin->getTheoTemplate(i).getName();

  bool ok;
  QString result = QInputDialog::getItem(this, "Save As Template", "Template Name", list, 0, false, &ok);
  if (!ok)
    return;

  if (result == "<new>") {
    QString text("");
    result = QInputDialog::getText(this, "New Template Name", "Enter Template Name", QLineEdit::Normal, text, &ok);
    if (!ok)
      return;
    // analyze the name: it has to start with 'T: '
    if (!result.startsWith("T: "))
      result.prepend("T: ");
    newTemplate = true;
  }

  // read musrfit_funcs.xml
  QString path = std::getenv("HOME");
  QString pathFln = path + "/.musrfit/musrWiz/musrfit_funcs.xml";
  bool found = false;
  if (QFile::exists(pathFln)) {
    found = true;
  }
  if (!found) {
    QMessageBox::critical(this, "ERROR", "Couldn't find musrfit_funcs.xml");
    return;
  }

  QFile fin(pathFln);
  if (!fin.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QMessageBox::critical(this, "ERROR", "Couldn't open musrfit_funcs.xml for reading.");
    return;
  }

  QTextStream in(&fin);
  QString data = in.readAll();
  fin.close();

  int idx, idxEnd;
  QString insertStr, errMsg;
  if (newTemplate) { // new template
    // first find the proper place where to insert the new template
    idx = data.lastIndexOf("</theo_template>");
    if (idx == -1) {
      QMessageBox::critical(this, "ERROR", "Couldn't find last theo_template. Something is very wrong.");
      return;
    }
    idx = data.indexOf("<!--", idx);
    if (idx == -1) {
      QMessageBox::critical(this, "ERROR", "Couldn't find last theo_template. Something is very wrong.");
      return;
    }
    idx -= 2;
  } else { // replace existing template
    // find template name
    idx = data.indexOf(result);
    if (idx == -1) {
      errMsg = QString("Couldn't find theo_template \"%1\". Something is very wrong.").arg(result);
      QMessageBox::critical(this, "ERROR", errMsg);
      return;
    }
    idx = data.lastIndexOf("  <theo_template>", idx);
    if (idx == -1) {
      QMessageBox::critical(this, "ERROR", "Couldn't find start theo_template. Something is very wrong.");
      return;
    }
    idxEnd = data.indexOf("  </theo_template>", idx);
    if (idxEnd == -1) {
      QMessageBox::critical(this, "ERROR", "Couldn't find end theo_template. Something is very wrong.");
      return;
    }
    idxEnd += 19;
    // remove the template which needs to be replaced
    data.remove(idx, idxEnd-idx);
  }
  insertStr = QString("");
  insertStr += "  <theo_template>\n";
  insertStr += QString("    <pre_def_name>%1</pre_def_name>\n").arg(result);
  QString theoStr = fMsrData->getTheory();
  theoStr.replace("\n", "\\n");
  insertStr += QString("    <theory>%1</theory>\n").arg(theoStr);
  for (int i=0; i<fMsrData->getNoOfFunc(); i++) {
    insertStr += QString("    <theo_fun>%1</theo_fun>\n").arg(fMsrData->getFunc(i));
  }
  for (int i=0; i<fMsrData->getNoOfMap(); i++) {
    insertStr += QString("    <theo_map no=\"%1\" name=\"%2\"/>\n").arg(fMsrData->getMap(i).getNumber()).arg(fMsrData->getMap(i).getName());
  }
  insertStr += "  </theo_template>\n";
  data.insert(idx, insertStr);

  // move original musrfit_funcs.xml -> musrfit_funcs.xml.backup
  QString pathFlnBackup = pathFln+QString(".backup");
  fin.rename(pathFlnBackup);

  // write new musrfit_funcs.xml
  QFile fout(pathFln);

  if (!fout.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QMessageBox::critical(this, "ERROR", "Couldn't open musrfit_funcs.xml for writing.");
    return;
  }

  QTextStream out(&fout);
  out << data;
  fout.close();
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * @brief PMusrWiz::PMusrWiz
 */
PMusrWiz::PMusrWiz(QWidget *parent) : QWizard(parent)
{
  fAdmin = 0;
  fMsrData = 0;

  QObject::connect(this, SIGNAL(finished(int)), this, SLOT(writeMsrFile(int)));
  QObject::connect(this, SIGNAL(helpRequested()), this, SLOT(help()));
}

//-------------------------------------------------------------------------
/**
 * @brief PMusrWiz::PMusrWiz
 */
PMusrWiz::PMusrWiz(PAdmin *admin, PMsrData *msrData, QWidget *parent) :
  QWizard(parent),
  fAdmin(admin),
  fMsrData(msrData)
{
#ifndef Q_OS_DARWIN
  setWizardStyle(ModernStyle);
#endif

  fMsrFilePath = QString("./");

  setPage(ePageIntro, new PIntroPage(fAdmin, fMsrData));
  setPage(ePageTheory, new PTheoPage(fAdmin, fMsrData));
  setPage(ePageMaps, new PMapPage(fMsrData));
  setPage(ePageFunctions, new PFuncPage(fMsrData));
  setPage(ePageParameters, new PParamPage(fMsrData));
  setPage(ePageFitInfo, new PFitInfoPage(fMsrData));
  setPage(ePageConclusion, new PConclusionPage(fAdmin, fMsrData, &fMsrFilePath));

  setStartId(ePageIntro);

  setOption(HaveHelpButton, true);

  setWindowTitle("msr-file Wizard");
  QString iconTheme = QIcon::themeName();
  bool isDark = false;
  if (iconTheme.contains("dark", Qt::CaseInsensitive))
    isDark = true;

  if (isDark)
    iconTheme = QString(":/icons/musrWiz-22x22-dark.svg");
  else
    iconTheme = QString(":/icons/musrWiz-22x22.svg");
  setWindowIcon(QIcon(QPixmap(iconTheme)));

  if (isDark)
    iconTheme = QString(":/icons/musrWiz-32x32-dark.svg");
  else
    iconTheme = QString(":/icons/musrWiz-32x32.svg");
  setPixmap(QWizard::LogoPixmap, QPixmap(iconTheme));

  QObject::connect(this, SIGNAL(finished(int)), this, SLOT(writeMsrFile(int)));
  QObject::connect(this, SIGNAL(helpRequested()), this, SLOT(help()));
}

//-------------------------------------------------------------------------
/**
 * @brief PMusrWiz::~PMusrWiz
 */
PMusrWiz::~PMusrWiz()
{

}

//-------------------------------------------------------------------------
/**
 * @brief PMusrWiz::writeMsrFile
 * @param result
 * @return
 */
int PMusrWiz::writeMsrFile(int result)
{
  if (result != QDialog::Accepted)
    return -1;

  int res = 0;

  // keep msr-file path-name
  fMsrData->setMsrPathName(fMsrFilePath);

  // no msr-file name given, hence generate it
  if (fMsrData->getMsrFileName() == QString("")) {
    if (fMsrData->getRunNumber() == -1) {
      QMessageBox::critical(this, "ERROR", "No run number given! Won't do anything.");
      return -2;
    }
    fMsrData->setMsrFileName(QString("%1").arg(fMsrData->getRunNumber()));

    if (fMsrData->getTypeOfMeasurement() == MEASURE_ZF)
      fMsrData->appendMsrFileName("_zf");
    else if (fMsrData->getTypeOfMeasurement() == MEASURE_TF)
      fMsrData->appendMsrFileName("_tf");
    else if (fMsrData->getTypeOfMeasurement() == MEASURE_LF)
      fMsrData->appendMsrFileName("_lf");

    if (fMsrData->getFitType() == FIT_TYPE_SINGLE_HISTO)
      fMsrData->appendMsrFileName("_histo");
    else if (fMsrData->getFitType() == FIT_TYPE_SINGLE_HISTO_RRF)
      fMsrData->appendMsrFileName("_histo_rrf");
    else if (fMsrData->getFitType() == FIT_TYPE_ASYMMETRY)
      fMsrData->appendMsrFileName("_asym");
    else if (fMsrData->getFitType() == FIT_TYPE_ASYMMETRY_RRF)
      fMsrData->appendMsrFileName("_asym_rrf");
    else if (fMsrData->getFitType() == FIT_TYPE_MU_MINUS)
      fMsrData->appendMsrFileName("_muMinus");
  }
  fMsrData->appendMsrFileName(".msr");

  if (fMsrData->getFitType() == FIT_TYPE_SINGLE_HISTO) {
    res = writeMsrFileSingleHisto();
  } else if (fMsrData->getFitType() == FIT_TYPE_ASYMMETRY) {
    res = writeMsrFileAsymmetry();
  } else {
    res = -99;
  }

  return res;
}

//-------------------------------------------------------------------------
/**
 * @brief PMusrWiz::writeMsrFileSingleHisto
 * @return
 */
int PMusrWiz::writeMsrFileSingleHisto()
{
  QString pathFileName = fMsrData->getMsrPathName() + "/" + fMsrData->getMsrFileName();
  QFile fln(pathFileName);

  if (!fln.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QMessageBox::critical(this, "ERROR", "Cannot open file for writting.");
    return -3;
  }

  QTextStream fout(&fln);
  QString line = QString("###############################################################");

  // write title
  fout << fMsrData->getMsrFileName() << Qt::endl;
  fout << line << Qt::endl;

  // write parameter block
  fout << "FITPARAMETER" << Qt::endl;
  fout << "#      Nr. Name       Value      Step       Pos_Error   Boundaries" << Qt::endl;
  PParam param;
  // global fit parameters
  for (int i=0; i<fMsrData->getNoOfParam(); i++) {
    param = fMsrData->getParam(i);
    fout << qSetFieldWidth(9);
    fout << Qt::right << param.getNumber();
    fout << qSetFieldWidth(0) << "  ";
    fout << qSetFieldWidth(11);
    fout << Qt::left << param.getName();
    fout << Qt::left << param.getValue();
    fout << Qt::left << param.getStep();
    fout << Qt::left << param.getPosErr();
    if (!param.getBoundLow().isEmpty())
      fout << Qt::left << param.getBoundLow();
    if (!param.getBoundHigh().isEmpty())
      fout << Qt::left << param.getBoundHigh();
    fout << qSetFieldWidth(0);
    fout << Qt::endl;
  }

  // detector specific fit parameters
  QString str;
  PParam map;

  // get proper detector information from the instrument definition
  PInstrument *instru = fAdmin->getInstrument(fMsrData->getInstitute(), fMsrData->getInstrument());
  if (instru == 0) {
    QString msg = QString("No setups found for %1: %2").arg(fMsrData->getInstitute(), fMsrData->getInstrument());
    QMessageBox::critical(this, "ERROR", msg);
    return -4;
  }
  PSetup *setup = 0;
  switch (fMsrData->getTypeOfMeasurement()) {
  case MEASURE_ZF:
    setup = instru->getZFSetup(fMsrData->getSetup());
    break;
  case MEASURE_TF:
    setup = instru->getTFSetup(fMsrData->getSetup());
    break;
  case MEASURE_LF:
    setup = instru->getLFSetup(fMsrData->getSetup());
    break;
  case MEASURE_UNDEF:
  default:
    break;
  }
  if (setup == 0) {
    QString msg = QString("No setups found for %1: %2: %3").arg(fMsrData->getInstitute(), fMsrData->getInstrument()).arg(fMsrData->getSetup());
    QMessageBox::critical(this, "ERROR", msg);
    return -5;
  }
  int noOfDetec = setup->getNoOfLogicalDetectors();

  PDetector *detector = 0;
  QString detectorName("");
  for (int i=0; i<noOfDetec; i++) {
    detector = setup->getDetector(i);
    detectorName = detector->getName();
    // name comment
    fout << "# " << detectorName << Qt::endl;

    // first all maps
    for (int j=0; j<fMsrData->getNoOfMap(); j++) {
      map = fMsrData->getMap(j);
      fout << qSetFieldWidth(9);
      fout << Qt::right << fMsrData->getNoOfParam() + 1 + j + (fMsrData->getNoOfMap()+2)*i;
      fout << qSetFieldWidth(0) << "  ";
      fout << qSetFieldWidth(11);
      str = map.getName() + QString("_%1").arg(detectorName);
      fout << Qt::left << str;
      if (map.getName().startsWith("ph", Qt::CaseInsensitive) ||
          map.getName().startsWith("relph", Qt::CaseInsensitive)) {
        fout << Qt::left << detector->getRelGeomPhase();
        // if RelPh is found, the first will be fixed to 0
        if (map.getName().startsWith("relph", Qt::CaseInsensitive) && (i==0))
          fout << Qt::left << 0.0;
        else
          fout << Qt::left << 12.3;
        fout << Qt::left << "none";
      } else {
        fout << Qt::left << map.getValue();
        fout << Qt::left << map.getStep();
        fout << Qt::left << map.getPosErr();
        if (map.getBoundLow() != "")
          fout << Qt::left << map.getBoundLow();
        if (map.getBoundHigh() != "")
          fout << Qt::left << map.getBoundHigh();
      }
      fout << qSetFieldWidth(0);
      fout << Qt::endl;
    }

    // write N0 and N_bkg
    fout << qSetFieldWidth(9);
    fout << Qt::right << fMsrData->getNoOfParam() + fMsrData->getNoOfMap() + 1 + (fMsrData->getNoOfMap()+2)*i;
    fout << qSetFieldWidth(0) << "  ";
    fout << qSetFieldWidth(11);
    str = QString("N0_%1").arg(detectorName);
    fout << Qt::left << str;
    fout << Qt::left << "123.4";
    fout << Qt::left << "1.0";
    fout << Qt::left << "none";
    fout << qSetFieldWidth(0);
    fout << Qt::endl;

    fout << qSetFieldWidth(9);
    fout << Qt::right << fMsrData->getNoOfParam() + fMsrData->getNoOfMap() + 2 + (fMsrData->getNoOfMap()+2)*i;
    fout << qSetFieldWidth(0) << "  ";
    fout << qSetFieldWidth(11);
    str = QString("N_bkg_%1").arg(detectorName);
    fout << Qt::left << str;
    fout << Qt::left << "1.234";
    fout << Qt::left << "0.1";
    fout << Qt::left << "none";
    fout << qSetFieldWidth(0);
    fout << Qt::endl;
  }

  fout.setFieldWidth(0);
  fout << Qt::endl << line << Qt::endl;

  // write theory block
  fout << "THEORY" << Qt::endl;
  fout << fMsrData->getTheory() << Qt::endl;
  fout << Qt::endl << line << Qt::endl;

  // write functions block
  if (fMsrData->getNoOfFunc() > 0) {
    fout << "FUNCTIONS" << Qt::endl;
    for (int i=0; i<fMsrData->getNoOfFunc(); i++) {
      fout << fMsrData->getFunc(fMsrData->getFuncNo(i)) << Qt::endl;
    }
    fout << Qt::endl << line << Qt::endl;
  }

  // write global block
  fout << "GLOBAL" << Qt::endl;
  fout << "fittype        " << fMsrData->getFitType()-1 << Qt::endl;
  fout << "fit            " << fMsrData->getFitStart() << "   " << fMsrData->getFitEnd() << Qt::endl;
  fout << "packing        " << fMsrData->getPacking() << Qt::endl;
  fout << Qt::endl << line << Qt::endl;

  // write run block(s)
  int t0 = 0;
  if (fMsrData->getT0Tag() == T0_ENTER_WIZ)
    t0 = fMsrData->getT0();
  int fgbOffset = setup->getFgbOffset();
  int lgb = setup->getLgb();
  QString runName = getRunName(instru);
  QVector<int> detectorNo;
  for (int i=0; i<noOfDetec; i++) {
    detector = setup->getDetector(i);
    detectorNo = detector->getForwards();
    fout << "RUN " << runName << Qt::endl;
    fout << "map         ";
    fout << qSetFieldWidth(10);
    for (int j=0; j<fMsrData->getNoOfMap(); j++)
       fout << Qt::left << fMsrData->getNoOfParam()+ 1 + (fMsrData->getNoOfMap()+2)*i + j;
    fout << qSetFieldWidth(0) << Qt::endl;
    fout << "forward     ";
    for (int j=0; j<detectorNo.size()-1; j++)
      fout << detectorNo[j] << "  ";
    fout << detectorNo[detectorNo.size()-1] << Qt::endl;
    fout << "norm        " << fMsrData->getNoOfParam() + fMsrData->getNoOfMap() + 1 + (fMsrData->getNoOfMap()+2)*i << Qt::endl;
    fout << "backgr.fit  " << fMsrData->getNoOfParam() + fMsrData->getNoOfMap() + 2 + (fMsrData->getNoOfMap()+2)*i << Qt::endl;
    if (fMsrData->getT0Tag() == T0_ENTER_WIZ) {
      fout << "data        " << t0+fgbOffset << "   " << lgb << Qt::endl;
      fout << "t0          " << t0 << Qt::endl;
    } else if (fMsrData->getT0Tag() == T0_FROM_MUSR_T0) {
      fout << "data        120  " << lgb << Qt::endl;
      fout << "t0          100  " << Qt::endl;
    }
    if (i<noOfDetec-1)
      fout << "#-----------------------------------------------" << Qt::endl;
    else
      fout << Qt::endl << line << Qt::endl;
  }

  // write command block
  fout << "COMMANDS" << Qt::endl;
  fout << fMsrData->getCmd() << Qt::endl;
  fout << Qt::endl << line << Qt::endl;

  // write plot block
  fout << "PLOT " << fMsrData->getFitType()-1 << Qt::endl;
  fout << "lifetimecorrection" << Qt::endl;
  fout << "runs   1-" << setup->getNoOfLogicalDetectors() << Qt::endl;
  fout << "range  0.0  " << fMsrData->getFitEnd() << " -0.35 0.35" << Qt::endl;
  fout << "view_packing " << fMsrData->getPacking() << Qt::endl;
  fout << Qt::endl << line << Qt::endl;

  // write fourier block
  fout << "FOURIER" << Qt::endl;
  fout << "units            MHz" << Qt::endl;
  fout << "fourier_power    12" << Qt::endl;
  fout << "apodization      NONE" << Qt::endl;
  fout << "plot             POWER" << Qt::endl;
  fout << Qt::endl << line << Qt::endl;

  // write statistic block
  fout << "STATISTIC --- " << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss") << Qt::endl;
  fout << "*** FIT DID NOT CONVERGE ***" << Qt::endl;

  fln.close();

  return 0;
}

//-------------------------------------------------------------------------
/**
 * @brief PMusrWiz::writeMsrFileAsymmetry
 * @return
 */
int PMusrWiz::writeMsrFileAsymmetry()
{
  QString pathFileName = fMsrData->getMsrPathName() + "/" + fMsrData->getMsrFileName();
  QFile fln(pathFileName);

  if (!fln.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QMessageBox::critical(this, "ERROR", "Cannot open file for writting.");
    return -3;
  }

  QTextStream fout(&fln);
  QString line = QString("###############################################################");

  // write title
  fout << fMsrData->getMsrFileName() << Qt::endl;
  fout << line << Qt::endl;

  // write parameter block
  fout << "FITPARAMETER" << Qt::endl;
  fout << "#      Nr. Name       Value      Step       Pos_Error   Boundaries" << Qt::endl;
  PParam param;
  // global fit parameters
  for (int i=0; i<fMsrData->getNoOfParam(); i++) {
    param = fMsrData->getParam(i);
    fout << qSetFieldWidth(9);
    fout << Qt::right << param.getNumber();
    fout << qSetFieldWidth(0) << "  ";
    fout << qSetFieldWidth(11);
    fout << Qt::left << param.getName();
    fout << Qt::left << param.getValue();
    fout << Qt::left << param.getStep();
    fout << Qt::left << param.getPosErr();
    if (!param.getBoundLow().isEmpty())
      fout << Qt::left << param.getBoundLow();
    if (!param.getBoundHigh().isEmpty())
      fout << Qt::left << param.getBoundHigh();
    fout << qSetFieldWidth(0);
    fout << Qt::endl;
  }

  // detector specific fit parameters
  QString str;
  PParam map;

  // get proper detector information from the instrument definition
  PInstrument *instru = fAdmin->getInstrument(fMsrData->getInstitute(), fMsrData->getInstrument());
  if (instru == 0) {
    QString msg = QString("No setups found for %1: %2").arg(fMsrData->getInstitute(), fMsrData->getInstrument());
    QMessageBox::critical(this, "ERROR", msg);
    return -4;
  }
  PSetup *setup = 0;
  switch (fMsrData->getTypeOfMeasurement()) {
  case MEASURE_ZF:
    setup = instru->getZFSetup(fMsrData->getSetup());
    break;
  case MEASURE_TF:
    setup = instru->getTFSetup(fMsrData->getSetup());
    break;
  case MEASURE_LF:
    setup = instru->getLFSetup(fMsrData->getSetup());
    break;
  case MEASURE_UNDEF:
  default:
    break;
  }
  if (setup == 0) {
    QString msg = QString("No setups found for %1: %2: %3").arg(fMsrData->getInstitute(), fMsrData->getInstrument()).arg(fMsrData->getSetup());
    QMessageBox::critical(this, "ERROR", msg);
    return -5;
  }
  int noOfDetec = setup->getNoOfLogicalAsymDetectors();

  PDetector *detector = 0;
  QString detectorName("");
  for (int i=0; i<noOfDetec; i++) {
    detector = setup->getAsymDetector(i);
    detectorName = detector->getName();
    // name comment
    fout << "# " << detectorName << Qt::endl;

    // first all maps

    // write Alpha (mandatory)
    fout << qSetFieldWidth(9);
    fout << Qt::right << fMsrData->getNoOfParam() + 1 + (fMsrData->getNoOfMap()+1)*i;
    fout << qSetFieldWidth(0) << "  ";
    fout << qSetFieldWidth(11);
    str = QString("Alpha_%1").arg(detectorName);
    fout << Qt::left << str;
    fout << Qt::left << detector->getAlpha();
    fout << Qt::left << "0.01";
    fout << Qt::left << "none";
    fout << qSetFieldWidth(0);
    fout << Qt::endl;

    // write user defined maps
    for (int j=0; j<fMsrData->getNoOfMap(); j++) {
      map = fMsrData->getMap(j);
      fout << qSetFieldWidth(9);
      fout << Qt::right << fMsrData->getNoOfParam() + 2 + j + (fMsrData->getNoOfMap()+1)*i;
      fout << qSetFieldWidth(0) << "  ";
      fout << qSetFieldWidth(11);
      str = map.getName() + QString("_%1").arg(detectorName);
      fout << Qt::left << str;
      if (map.getName().startsWith("ph", Qt::CaseInsensitive) ||
          map.getName().startsWith("relph", Qt::CaseInsensitive)) {
        fout << Qt::left << detector->getRelGeomPhase();
        if (map.getName().startsWith("relph", Qt::CaseInsensitive) && (i==0))
          fout << Qt::left << 0.0;
        else
          fout << Qt::left << 12.3;
        fout << Qt::left << "none";
      } else {
        fout << Qt::left << map.getValue();
        fout << Qt::left << map.getStep();
        fout << Qt::left << map.getPosErr();
        if (map.getBoundLow() != "")
          fout << Qt::left << map.getBoundLow();
        if (map.getBoundHigh() != "")
          fout << Qt::left << map.getBoundHigh();
      }
      fout << qSetFieldWidth(0);
      fout << Qt::endl;
    }
  }

  fout.setFieldWidth(0);
  fout << Qt::endl << line << Qt::endl;

  // write theory block
  fout << "THEORY" << Qt::endl;
  fout << fMsrData->getTheory();
  fout << Qt::endl << line << Qt::endl;

  // write functions block
  if (fMsrData->getNoOfFunc() > 0) {
    fout << "FUNCTIONS" << Qt::endl;
    for (int i=0; i<fMsrData->getNoOfFunc(); i++) {
      fout << fMsrData->getFunc(fMsrData->getFuncNo(i)) << Qt::endl;
    }
    fout << Qt::endl << line << Qt::endl;
  }

  // write global block
  fout << "GLOBAL" << Qt::endl;
  fout << "fittype        " << fMsrData->getFitType()-1 << Qt::endl;
  fout << "fit            " << fMsrData->getFitStart() << "   " << fMsrData->getFitEnd() << Qt::endl;
  fout << "packing        " << fMsrData->getPacking() << Qt::endl;
  fout << Qt::endl << line << Qt::endl;

  // write run block(s)
  int t0 = 0;
  if (fMsrData->getT0Tag() == T0_ENTER_WIZ)
    t0 = fMsrData->getT0();
  int fgbOffset = setup->getFgbOffset();
  int lgb = setup->getLgb();
  QString runName = getRunName(instru);
  QVector<int> detectorNoF, detectorNoB;
  for (int i=0; i<noOfDetec; i++) {
    detector = setup->getAsymDetector(i);
    detectorNoF = detector->getForwards();
    detectorNoB = detector->getBackwards();
    fout << "RUN " << runName << Qt::endl;
    fout << "alpha       " << fMsrData->getNoOfParam() + 1 + (fMsrData->getNoOfMap()+1)*i << Qt::endl;

    fout << "map         ";
    fout << qSetFieldWidth(10);
    if (fMsrData->getNoOfMap() == 0) {
      fout << Qt::left << "0    0    0    0    0    0    0    0    0    0";
    } else {
      for (int j=0; j<fMsrData->getNoOfMap(); j++)
        fout << Qt::left << fMsrData->getNoOfParam() + 2 + (fMsrData->getNoOfMap()+1)*i + j;
    }
    fout << qSetFieldWidth(0) << Qt::endl;
    fout << "forward     ";
    for (int j=0; j<detectorNoF.size()-1; j++)
      fout << detectorNoF[j] << "  ";
    fout << detectorNoF[detectorNoF.size()-1] << Qt::endl;
    fout << "backward    ";
    for (int j=0; j<detectorNoB.size()-1; j++)
      fout << detectorNoB[j] << "  ";
    fout << detectorNoB[detectorNoB.size()-1] << Qt::endl;
    fout << "background  " << setup->getBkgStartBin() << "      " << setup->getBkgEndBin() << "    " << setup->getBkgStartBin() << "      " << setup->getBkgEndBin() << Qt::endl;
    if (fMsrData->getT0Tag() == T0_ENTER_WIZ) {
      fout << "data        " << t0+fgbOffset << "   " << lgb << "    " << t0+fgbOffset << "   " << lgb << "  " << t0+fgbOffset << "   " << lgb << "    " << t0+fgbOffset << "   " << lgb << Qt::endl;
      fout << "t0          " << t0 << "   " << t0 << Qt::endl;
    } else if (fMsrData->getT0Tag() == T0_FROM_MUSR_T0) { // musrt0 shall be called
      fout << "data        120  " << lgb << "   120  " << lgb << Qt::endl;
      fout << "t0          100     100" << Qt::endl;
    }
    if (i<noOfDetec-1)
      fout << "#-----------------------------------------------" << Qt::endl;
    else
      fout << Qt::endl << line << Qt::endl;
  }

  // write command block
  fout << "COMMANDS" << Qt::endl;
  fout << fMsrData->getCmd() << Qt::endl;
  fout << Qt::endl << line << Qt::endl;

  // write plot block
  fout << "PLOT " << fMsrData->getFitType()-1 << Qt::endl;
  fout << "lifetimecorrection" << Qt::endl;
  if (setup->getNoOfLogicalAsymDetectors() > 1)
    fout << "runs   1-" << setup->getNoOfLogicalAsymDetectors() << Qt::endl;
  else
    fout << "runs   1" << Qt::endl;
  fout << "range  0.0  " << fMsrData->getFitEnd() << " -0.35 0.35" << Qt::endl;
  fout << "view_packing " << fMsrData->getPacking() << Qt::endl;
  fout << Qt::endl << line << Qt::endl;

  // write fourier block
  fout << "FOURIER" << Qt::endl;
  fout << "units            MHz" << Qt::endl;
  fout << "fourier_power    12" << Qt::endl;
  fout << "apodization      NONE" << Qt::endl;
  fout << "plot             POWER" << Qt::endl;
  fout << Qt::endl << line << Qt::endl;

  // write statistic block
  fout << "STATISTIC --- " << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss") << Qt::endl;
  fout << "*** FIT DID NOT CONVERGE ***" << Qt::endl;

  fln.close();

  return 0;
}

//-------------------------------------------------------------------------
/**
 * @brief PMusrWiz::getRunName
 * @param instru
 * @return
 */
QString PMusrWiz::getRunName(PInstrument *instru)
{
  // prepare year strings (4 digits and 2 digits)
  QString year = fMsrData->getYear();
  QString yearShort = year.right(2);

  // check how many digits are needed for the run number (minimum required == 3)
  QString runTemplate = instru->getRunNameTemplate();
  int noOfDigits = 4;
  int idx = runTemplate.indexOf("NNNN");
  int count = 0;
  if (idx != -1) {
    do {
      count++;
    } while (runTemplate[idx+count] == 'N');
  }
  if (count >= 4)
    noOfDigits = count;

  // replace year strings from template
  while ((idx = runTemplate.indexOf("YYYY")) != -1) {
    runTemplate.replace(idx, 4, year);
  }
  while ((idx = runTemplate.indexOf("YY")) != -1) {
    runTemplate.replace(idx, 2, yearShort);
  }

  // replace run number
  QString runNo = QString("%1").arg(fMsrData->getRunNumber(), noOfDigits, 10, QLatin1Char('0'));
  QString runNoTemplate = "";
  for (int i=0; i<noOfDigits; i++)
    runNoTemplate += "N";
  while ((idx = runTemplate.indexOf(runNoTemplate)) != -1) {
    runTemplate.replace(idx, noOfDigits, runNo);
  }

  // add beamline info
  runTemplate += " " + instru->getBeamline() + " ";

  // add institute info
  runTemplate += fMsrData->getInstitute() + " ";

  // add run data format
  runTemplate += instru->getDataFileFormat();

  return runTemplate;
}

//-------------------------------------------------------------------------
/**
 * @brief PMusrWiz::help
 */
void PMusrWiz::help()
{
  QMessageBox::information(this, "Help", "Eventually there will be a help here.");
}
