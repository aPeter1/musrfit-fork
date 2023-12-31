/****************************************************************************

  PAdmin.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

*****************************************************************************/

/***************************************************************************
 *   Copyright (C) 2010-2023 by Andreas Suter                              *
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

#include <cstdlib>
#include <iostream>
using namespace std;

#include <QMessageBox>
#include <QString>
#include <QStringView>
#include <QFile>
#include <QTextStream>
#include <QVector>
#include <QXmlStreamAttributes>

#include <QtDebug>

#include "PAdmin.h"

//--------------------------------------------------------------------------
// implementation of PFuncXMLParser class
//--------------------------------------------------------------------------
/**
 * <p>XML Parser class for the musrWiz function file.
 *
 * \param admin pointer to an admin class instance.
 */
PFuncXMLParser::PFuncXMLParser(const QString& fln, PAdmin *admin) : fAdmin(admin)
{
  fValid = false;
  fKeyWord = eEmpty;

  QFile file(fln);
  if (!file.open(QFile::ReadOnly | QFile::Text)) {
    // warning and create default - STILL MISSING
  }

  fValid = parse(&file);
}

//--------------------------------------------------------------------------
/**
 * <p>parse musrfit_funcs.xml
 *
 * \param device QFile object of musrfit_funcs.xml
 *
 * @return true on success, false otherwise
 */
bool PFuncXMLParser::parse(QIODevice *device)
{
  fXml.setDevice(device);

  bool expectChars = false;
  while (!fXml.atEnd()) {
    fXml.readNext();
    if (fXml.isStartDocument()) {
      startDocument();
    } else if (fXml.isStartElement()) {
      startElement();
      expectChars = true;
    } else if (fXml.isCharacters() && expectChars) {
      characters();
    } else if (fXml.isEndElement()) {
      endElement();
      expectChars = false;
    } else if (fXml.isEndDocument()) {
      endDocument();
    }
  }
  if (fXml.hasError()) {
    QString msg;
    msg = QString("%1 Line %2, column %3").arg(fXml.errorString()).arg(fXml.lineNumber()).arg(fXml.columnNumber());
    QMessageBox::critical(nullptr, "ERROR", msg, QMessageBox::Ok, QMessageBox::NoButton);
    return false;
  }

  return true;
}

//--------------------------------------------------------------------------
/**
 * <p>Routine called at the beginning of the XML parsing process.
 */
bool PFuncXMLParser::startDocument()
{
  // nothing to be done here for now
  return true;
}

//--------------------------------------------------------------------------
/**
 * <p>Routine called when a new XML tag is found. Here it is used
 * to set a tag for filtering afterwards the content.
 */
bool PFuncXMLParser::startElement()
{
  QString qName = fXml.name().toString();
  QString str;

  QString errMsg("");
  int ival;
  double dval;
  bool ok;

  if (qName == "theo_template") {
    fTheoTemplate.init();
  } else if (qName == "pre_def_name") {
    fKeyWord = eTemplateName;
  } else if (qName == "theory") {
    fKeyWord = eTemplateTheo;
  } else if (qName == "theo_fun") {
    fKeyWord = eTemplateFunc;
  } else if (qName == "theo_map") {
    QXmlStreamAttributes qAttr = fXml.attributes();
    if (qAttr.count() != 2) {
      errMsg = QString("theo_map should have 2 attributes, called 'no', and 'name', found %1").arg(qAttr.count());
      QMessageBox::critical(nullptr, "ERROR", errMsg);
      return false;
    }
    PParam map;
    // get map number
    str = qAttr.value("no").toString();
    ival = str.toInt(&ok);
    if (!ok) {
      errMsg = QString("theo_map attribute 'no' is not a number (%1)").arg(str);
      QMessageBox::critical(nullptr, "ERROR", errMsg);
      return false;
    }
    map.setNumber(ival);
    // get map name
    str = qAttr.value("name").toString();
    map.setName(str);

    // check that all necessary attributes where found
    if ((map.getName() == "UnDef") || (map.getNumber() == -1)) {
      errMsg = QString("found theo_map with missing attribute(s)");
      QMessageBox::critical(nullptr, "ERROR", errMsg);
      return false;
    }
    fTheoTemplate.appendMap(map);
  } else if (qName == "template_param") {
    QXmlStreamAttributes qAttr = fXml.attributes();
    if ((qAttr.count() != 4) && (qAttr.count() != 6)) {
      errMsg = QString("template_param should have 4 or 6 attributes, called\n'no', 'name', 'value', 'step', ['boundLow', 'boundHigh'] found %1").arg(qAttr.count());
      QMessageBox::critical(nullptr, "ERROR", errMsg);
      return false;
    }
    PParam param;
    // parameter number
    str = qAttr.value("no").toString();
    ival = str.toInt(&ok);
    if (!ok) {
      errMsg = QString("template_param attribute 'no' is not a number (%1)").arg(str);
      QMessageBox::critical(nullptr, "ERROR", errMsg);
      return false;
    }
    param.setNumber(ival);
    // parameter name
    str = qAttr.value("name").toString();
    param.setName(str);
    // parameter value
    str = qAttr.value("value").toString();
    dval = str.toDouble(&ok);
    if (!ok) {
      errMsg = QString("template_param attribute 'value' is not a number (%1)").arg(str);
      QMessageBox::critical(nullptr, "ERROR", errMsg);
      return false;
    }
    param.setValue(dval);
    // parameter step
    str = qAttr.value("step").toString();
    dval = str.toDouble(&ok);
    if (!ok) {
      errMsg = QString("template_param attribute 'step' is not a number (%1)").arg(str);
      QMessageBox::critical(nullptr, "ERROR", errMsg);
      return false;
    }
    param.setStep(dval);
    // parameter lower bound
    str = qAttr.value("boundLow").toString();
    if (!str.isEmpty())
      param.setBoundLow(str);
    // parameter upper bound
    str = qAttr.value("boundHigh").toString();
    if (!str.isEmpty())
      param.setBoundHigh(str);
    fTheoTemplate.appendParam(param);
  } else if (qName == "func") {
    fFunc.initFunc();
  } else if (qName == "name") {
    fKeyWord = eName;
  } else if (qName == "abbrv") {
    fKeyWord = eAbbrv;
  } else if (qName == "no_of_parameters") {
    fKeyWord = eNoOfParam;
  } else if (qName == "parameter") {
    fParam.initParam();
  } else if (qName == "param_name") {
    fKeyWord = eParamName;
  } else if (qName == "param_value") {
    fKeyWord = eParamValue;
  } else if (qName == "param_is_map") {
    fKeyWord = eParamMap;
  }

  return true;
}

//--------------------------------------------------------------------------
/**
 * <p>Routine called when the end XML tag is found. It is used to
 * put the filtering tag to 'empty'. It also resets the fFunc flag in case
 * the entry was a theory function.
 */
bool PFuncXMLParser::endElement()
{
  fKeyWord = eEmpty;

  QString qName = fXml.name().toString();

  if (qName == "theo_template") {
    fAdmin->fTheoTemplate.push_back(fTheoTemplate);
  } else if (qName == "func") {
    fAdmin->fMusrfitFunc.push_back(fFunc);
  } else if (qName == "parameter") {
    fFunc.addFuncParam(fParam);
  }

  return true;
}

//--------------------------------------------------------------------------
/**
 * <p>This routine delivers the content of an XML tag. It fills the
 * content into the load data structure.
 */
bool PFuncXMLParser::characters()
{
  QString str = fXml.text().toString();
  if (str.isEmpty())
    return true;

  bool ok;
  int ival;
  double dval;

  switch (fKeyWord) {
  case eTemplateName:
    fTheoTemplate.setName(str);
    break;
  case eTemplateTheo:
    fTheoTemplate.setTheory(str);
    break;
  case eTemplateFunc:
    fTheoTemplate.appendFunc(str);
    break;
  case eName:
    fFunc.setName(str);
    break;
  case eAbbrv:
    fFunc.setAbbrv(str);
    break;
  case eNoOfParam:
    ival = str.toInt(&ok);
    if (ok)
      fFunc.setNoOfParam(ival);
    break;
  case eParamName:
    fParam.setParamName(str);
    break;
  case eParamValue:
    dval = str.toDouble(&ok);
    if (ok)
      fParam.setParamValue(dval);
    break;
  case eParamMap:
    if ((str == "y") || (str == "yes") || (str == "1"))
      fParam.setParamMap(true);
    break;
  default:
    break;
  }

  return true;
}

//--------------------------------------------------------------------------
/**
 * <p>Called at the end of the XML parse process. It checks if default paths
 * contain system variables, and if so expand them for the further use.
 */
bool PFuncXMLParser::endDocument()
{
  return true;
}

//--------------------------------------------------------------------------
// implementation of PInstrumentDefXMLParser class
//--------------------------------------------------------------------------
/**
 * <p>XML Parser class for the instrument definition file(s).
 *
 * \param file name of the instrument definition file(s).
 * \param admin pointer to an admin class instance.
 */
PInstrumentDefXMLParser::PInstrumentDefXMLParser(const QString& fln, PAdmin *admin) : fAdmin(admin)
{
  fValid = false;
  fKeyWord = eEmpty;

  fInstituteName = "";
  fInstrument = 0;
  fSetup = 0;

  QFile file(fln);
  if (!file.open(QFile::ReadOnly | QFile::Text)) {
    // warning and create default - STILL MISSING
  }

  fValid = parse(&file);
}

//--------------------------------------------------------------------------
/**
 * <p>parse the instrument definition xml-file(s).
 *
 * \param device QFile object of the instrument definition xml-file(s).
 *
 * @return true on success, false otherwise
 */
bool PInstrumentDefXMLParser::parse(QIODevice *device)
{
  fXml.setDevice(device);

  bool expectChars = false;
  while (!fXml.atEnd()) {
    fXml.readNext();
    if (fXml.isStartDocument()) {
      startDocument();
    } else if (fXml.isStartElement()) {
      startElement();
      expectChars = true;
    } else if (fXml.isCharacters() && expectChars) {
      characters();
    } else if (fXml.isEndElement()) {
      endElement();
      expectChars = false;
    } else if (fXml.isEndDocument()) {
      endDocument();
    }
  }
  if (fXml.hasError()) {
    QString msg;
    msg = QString("%1 Line %2, column %3").arg(fXml.errorString()).arg(fXml.lineNumber()).arg(fXml.columnNumber());
    QMessageBox::critical(nullptr, "ERROR", msg, QMessageBox::Ok, QMessageBox::NoButton);
    return false;
  }

  return true;
}

//--------------------------------------------------------------------------
/**
 * <p>Routine called at the beginning of the XML parsing process.
 */
bool PInstrumentDefXMLParser::startDocument()
{
  // nothing to be done here for now
  return true;
}

//--------------------------------------------------------------------------
/**
 * <p>Routine called when a new XML tag is found. Here it is used
 * to set a tag for filtering afterwards the content.
 */
bool PInstrumentDefXMLParser::startElement()
{
  QString qName = fXml.name().toString();

  bool ok;
  double dval;
  double ival;
  QString str, errMsg;
  QStringList strL;
  QVector<int> forward;
  QVector<int> backward;

  if (qName == "institute") {
    fKeyWord = eInstitute;
  } else if (qName == "instrument") {
    fKeyWord = eInstrument;
    QXmlStreamAttributes qAttr = fXml.attributes();
    if (qAttr.count() != 1) {
      errMsg = QString("instrument should have 1 attribute, called 'name', found %1").arg(qAttr.count());
      QMessageBox::critical(nullptr, "ERROR", errMsg);
      return false;
    }
    if (fInstituteName == "") {
      errMsg = QString("found instrument without institute set.");
      QMessageBox::critical(nullptr, "ERROR", errMsg);
      return false;
    }
    // create an instrument object
    fInstrument = new PInstrument();
    fInstrument->setInstitue(fInstituteName);
    fInstrument->setName(qAttr.value("name").toString());
  } else if (qName == "run_name_template") {
    fKeyWord = eRunNameTemplate;
  } else if (qName == "beamline") {
    fKeyWord = eBeamline;
  } else if (qName == "data_file_format") {
    fKeyWord = eDataFileFormat;
  } else if (qName == "tf") {
    fKeyWord = eTf;
    QXmlStreamAttributes qAttr = fXml.attributes();
    fSetup = new PSetup();
    if (qAttr.count() == 1)
      fSetup->setName(qAttr.value("name").toString());
    else
      fSetup->setName("Default");
  } else if (qName == "zf") {
    fKeyWord = eZf;
    QXmlStreamAttributes qAttr = fXml.attributes();
    fSetup = new PSetup();
    if (qAttr.count() == 1)
      fSetup->setName(qAttr.value("name").toString());
    else
      fSetup->setName("Default");
  } else if (qName == "lf") {
    fKeyWord = eLf;
    QXmlStreamAttributes qAttr = fXml.attributes();
    fSetup = new PSetup();
    if (qAttr.count() == 1)
      fSetup->setName(qAttr.value("name").toString());
    else
      fSetup->setName("Default");
  } else if (qName == "no_of_detectors") {
    fKeyWord = eNoOfDetectors;
  } else if (qName == "fgb_offset") {
    fKeyWord = eFgbOffset;
  } else if (qName == "lgb") {
    fKeyWord = eLgb;
  } else if (qName == "asym_bkg_range") {
    fKeyWord = eBkgRange;
  } else if (qName == "logic_detector") {
    QXmlStreamAttributes qAttr = fXml.attributes();
    if (qAttr.count() < 3)
      return false;
    PDetector detect;
    // detector name
    str = qAttr.value("name").toString();
    detect.setName(str);
    // detector relative phase
    str = qAttr.value("rel_phase").toString();
    dval = str.toDouble(&ok);
    if (!ok) {
      return false;
    }
    detect.setRelGeomPhase(dval);
    // detector number(s)
    str = qAttr.value("forward").toString();
    strL.clear();
    strL = str.split(' ');
    forward.clear();
    for (int j=0; j<strL.size(); j++) {
      ival = strL[j].toInt(&ok);
      if (!ok)
        return false;
      forward.push_back(ival);
    }
    detect.setForwards(forward);
    fSetup->addDetector(detect);
  } else if (qName == "logic_asym_detector") {
    QXmlStreamAttributes qAttr = fXml.attributes();
    if (qAttr.count() != 5)
      return false;
    PDetector detect;
    // detector name
    str = qAttr.value("name").toString();
    detect.setName(str);
    // detector relative phase
    str = qAttr.value("rel_phase").toString();
    dval = str.toDouble(&ok);
    if (!ok) {
      return false;
    }
    detect.setRelGeomPhase(dval);
    // detector forward
    str = qAttr.value("forward").toString();
    strL.clear();
    strL = str.split(' ');
    forward.clear();
    for (int j=0; j<strL.size(); j++) {
      ival = strL[j].toInt(&ok);
      if (!ok)
        return false;
      forward.push_back(ival);
    }
    detect.setForwards(forward);
    // detector backward
    str = qAttr.value("backward").toString();
    strL.clear();
    strL = str.split(' ');
    backward.clear();
    for (int j=0; j<strL.size(); j++) {
      ival = strL[j].toInt(&ok);
      if (!ok)
        return false;
      backward.push_back(ival);
    }
    detect.setBackwards(backward);
    // detector alpha
    str = qAttr.value("alpha").toString();
    dval = str.toDouble(&ok);
    if (!ok)
      return false;
    detect.setAlpha(dval);
    fSetup->addAsymDetector(detect);
  }

  return true;
}

//--------------------------------------------------------------------------
/**
 * <p>Routine called when the end XML tag is found. It is used to
 * put the filtering tag to 'empty'. It also resets the fFunc flag in case
 * the entry was a theory function.
 */
bool PInstrumentDefXMLParser::endElement()
{
  fKeyWord = eEmpty;

  QString qName = fXml.name().toString();

  if (qName == "instrument") {
    // store instrument
    fAdmin->addInstrument(fInstituteName, *fInstrument);

    // delete instrument object
    if (fInstrument) {
      delete fInstrument;
      fInstrument = 0;
    }
  } else if (qName == "zf") {
    // store setup
    fInstrument->addSetupZF(*fSetup);

    // delete setup object
    if (fSetup) {
      delete fSetup;
      fSetup = 0;
    }
  } else if (qName == "tf") {
    // store setup
    fInstrument->addSetupTF(*fSetup);

    // delete setup object
    if (fSetup) {
      delete fSetup;
      fSetup = 0;
    }
  } else if (qName == "lf") {
    // store setup
    fInstrument->addSetupLF(*fSetup);

    // delete setup object
    if (fSetup) {
      delete fSetup;
      fSetup = 0;
    }
  }

  return true;
}

//--------------------------------------------------------------------------
/**
 * <p>This routine delivers the content of an XML tag. It fills the
 * content into the load data structure.
 */
bool PInstrumentDefXMLParser::characters()
{
  QString str = fXml.text().toString();
  if (str.isEmpty())
    return true;

  bool ok;
  int ival, start, end;
  QString errMsg;
  QStringList strList;

  switch (fKeyWord) {
  case eInstitute:
    fInstituteName = str;
    break;
  case eRunNameTemplate:
    fInstrument->setRunNameTemplate(str);
    break;
  case eBeamline:
    fInstrument->setBeamline(str);
    break;
  case eDataFileFormat:
    fInstrument->setDataFileFormat(str);
    break;
  case eNoOfDetectors:
    if (fSetup == 0) {
      errMsg = "setup object not found.";
      QMessageBox::critical(nullptr, "ERROR", errMsg);
      return false;
    }
    ival = str.toInt(&ok);
    if (!ok) {
      errMsg = QString("Setup Error: No of Detectors = '%1', which is not an int.").arg(str);
      QMessageBox::critical(nullptr, "ERROR", errMsg);
      return false;
    }
    fSetup->setNoOfDetectors(ival);
    break;
  case eFgbOffset:
    if (fSetup == 0)
      return false;
    ival = str.toInt(&ok);
    if (!ok)
      return false;
    fSetup->setFgbOffset(ival);
    break;
  case eLgb:
    if (fSetup == 0)
      return false;
    ival = str.toInt(&ok);
    if (!ok)
      return false;
    fSetup->setLgb(ival);
    break;
  case eBkgRange:
    strList = str.split(' ', Qt::SkipEmptyParts);
    if (strList.size() != 2) {
      errMsg = QString("Found wrong Asymmetry background range: '%1'").arg(str);
      QMessageBox::critical(nullptr, "ERROR", errMsg);
      return false;
    }
    start = strList[0].toInt(&ok);
    if (!ok) {
      return false;
    }
    end = strList[1].toInt(&ok);
    if (!ok) {
      return false;
    }
    fSetup->setBkgRange(start, end);
    break;
  default:
    break;
  }

  return true;
}

//--------------------------------------------------------------------------
/**
 * <p>Called at the end of the XML parse process. It checks if default paths
 * contain system variables, and if so expand them for the further use.
 */
bool PInstrumentDefXMLParser::endDocument()
{
  if (fInstituteName == "") {
    QMessageBox::critical(nullptr, "FATAL ERROR", "Didn't find any institute name in the instrument definitions.");
    return false;
  }

  return true;
}

//--------------------------------------------------------------------------
// implementation of PMusrWizDefault class
//--------------------------------------------------------------------------
/**
 * <p>Initializes that PMusrWizDefault object.
 */
PMusrWizDefault::PMusrWizDefault()
{
  fInstitute = "UnDef";
  fInstrument = "UnDef";
  fFitType = "UnDef";
}

//--------------------------------------------------------------------------
// implementation of PMusrWizDefaultXMLParser class
//--------------------------------------------------------------------------
/**
 * <p>XML Parser class for the musrWiz default settings.
 *
 * \param admin pointer to an admin class instance.
 */
PMusrWizDefaultXMLParser::PMusrWizDefaultXMLParser(const QString& fln, PAdmin *admin) : fAdmin(admin)
{
  fValid = false;
  fKeyWord = eEmpty;

  QFile file(fln);
  if (!file.open(QFile::ReadOnly | QFile::Text)) {
    // warning and create default - STILL MISSING
  }

  fValid = parse(&file);
}

//--------------------------------------------------------------------------
/**
 * <p>parse the musrWiz startup xml-file.
 *
 * \param device QFile object of the musrWiz startup xml-file
 *
 * @return true on success, false otherwise
 */
bool PMusrWizDefaultXMLParser::parse(QIODevice *device)
{
  fXml.setDevice(device);

  bool expectChars = false;
  while (!fXml.atEnd()) {
    fXml.readNext();
    if (fXml.isStartDocument()) {
      startDocument();
    } else if (fXml.isStartElement()) {
      startElement();
      expectChars = true;
    } else if (fXml.isCharacters() && expectChars) {
      characters();
    } else if (fXml.isEndElement()) {
      endElement();
      expectChars = false;
    } else if (fXml.isEndDocument()) {
      endDocument();
    }
  }
  if (fXml.hasError()) {
    QString msg;
    msg = QString("%1 Line %2, column %3").arg(fXml.errorString()).arg(fXml.lineNumber()).arg(fXml.columnNumber());
    QMessageBox::critical(nullptr, "ERROR", msg, QMessageBox::Ok, QMessageBox::NoButton);
    return false;
  }

  return true;
}

//--------------------------------------------------------------------------
/**
 * <p>Routine called at the beginning of the XML parsing process.
 */
bool PMusrWizDefaultXMLParser::startDocument()
{
  // nothing to be done here for now
  return true;
}

//--------------------------------------------------------------------------
/**
 * <p>Routine called when a new XML tag is found. Here it is used
 * to set a tag for filtering afterwards the content.
 */
bool PMusrWizDefaultXMLParser::startElement()
{
  QString qName = fXml.name().toString();

  if (qName == "institute") {
    fKeyWord = eInstitute;
  } else if (qName == "instrument") {
    fKeyWord = eInstrument;
  } else if (qName == "fit_type") {
    fKeyWord = eFitType;
  }

  return true;
}

//--------------------------------------------------------------------------
/**
 * <p>Routine called when the end XML tag is found. It is used to
 * put the filtering tag to 'empty'. It also resets the fFunc flag in case
 * the entry was a theory function.
 */
bool PMusrWizDefaultXMLParser::endElement()
{
  fKeyWord = eEmpty;

  return true;
}

//--------------------------------------------------------------------------
/**
 * <p>This routine delivers the content of an XML tag. It fills the
 * content into the load data structure.
 */
bool PMusrWizDefaultXMLParser::characters()
{
  QString str = fXml.text().toString();
  if (str.isEmpty())
    return true;

  switch (fKeyWord) {
  case eInstitute:
    fDefault.setInstitute(str);
    break;
  case eInstrument:
    fDefault.setInstrument(str);
    break;
  case eFitType:
    fDefault.setFitType(str);
    break;
  default:
    break;
  }

  return true;
}

//--------------------------------------------------------------------------
/**
 * <p>Called at the end of the XML parse process.
 */
bool PMusrWizDefaultXMLParser::endDocument()
{
  fAdmin->fDefault = fDefault;

  return true;
}

//--------------------------------------------------------------------------
// implementation of PAdmin class
//--------------------------------------------------------------------------
/**
 * <p>Initializes that PAdmin object, and calls the XML parser which feeds
 * the object variables.
 */
PAdmin::PAdmin() : QObject()
{
  QString path, fln, pathFln;
  int count = 0;

  fValid = true;

  // load musrWiz default settings
  // 1st: check local directory
  pathFln = QString("./musrWiz.xml");
  if (!QFile::exists(pathFln)) {
    // 2nd: check $HOME/.musrfit/musrWiz/musrWiz.xml
    path = std::getenv("HOME");
    pathFln = path + "/.musrfit/musrWiz/musrWiz.xml";
    if (loadMusrWizDefault(pathFln)) {
      QMessageBox::warning(nullptr, "WARNING", "Couldn't find musrWiz.xml file.");
    }
  }

  // load musrfit funcs
  // 1st: check local directory
  pathFln = QString("./musrfit_funcs.xml");
  if (!QFile::exists(pathFln)) {
    // 2nd: check $HOME/.musrfit/musrWiz/musrfit_funcs.xml
    path = std::getenv("HOME");
    pathFln = path + "/.musrfit/musrWiz/musrfit_funcs.xml";
    if (loadMusrfitFunc(pathFln)) {
      fValid = false;
      QMessageBox::critical(nullptr, "FATAL ERROR", "Couldn't find any musrfit function definitions.");
      return;
    }
  }

  // load instrument definitions
  QStringList instStr;
  instStr << "psi" << "triumf" << "isis" << "jparc";

  for (int i=0; i<instStr.size(); i++) {
    // XML Parser part
    // 1st: check local directory
    path = QString("./");
    fln = QString("instrument_def_%1.xml").arg(instStr[i]);
    pathFln = path + fln;
    if (!QFile::exists(pathFln)) {
      // 2nd: check $HOME/.musrfit/musrWiz/instrument_def_XXX.xml
      path = std::getenv("HOME");
      path += "/.musrfit/musrWiz/";
      if (loadInstrumentDef(path, fln)) {
        fValid = false;
        return;
      } else {
        count++;
      }
    }
  }

  if (count == 0) {
    QMessageBox::critical(nullptr, "FATAL ERROR", "Couldn't find any instrument definition.");
    fValid = false;
  }
}

//--------------------------------------------------------------------------
/**
 * @brief PAdmin::getInstituteList
 * @return
 */
QStringList PAdmin::getInstituteList()
{
  QStringList list;

  for (int i=0; i<fInstitute.size(); i++) {
    list << fInstitute[i].getName();
  }

  return list;
}

//--------------------------------------------------------------------------
QStringList PAdmin::getInstrumentList(QString institute)
{
  QStringList list;

  // find institute
  int idx=-1;
  for (int i=0; i<fInstitute.size(); i++) {
    if (fInstitute[i].getName() == institute) {
      idx = i;
      break;
    }
  }

  // institute not found
  if (idx == -1)
    return list;

  // create instrument list
  QVector<PInstrument> instrument = fInstitute[idx].getInstruments();
  for (int i=0; i<instrument.size(); i++) {
    list << instrument[i].getName();
  }

  return list;
}

//--------------------------------------------------------------------------
/**
 * @brief PAdmin::addInstrument
 * @param institute
 * @param instrument
 * @return
 */
int PAdmin::addInstrument(QString institute, PInstrument instrument)
{
  // find institute
  int idx = -1;
  for (int i=0; i<fInstitute.size(); i++) {
    if (fInstitute[i].getName() == institute) {
      idx = i;
      break;
    }
  }

  if (idx == -1) {
    PInstitute inst;
    inst.setName(institute);
    fInstitute.push_back(inst);
    idx = fInstitute.size()-1;
  }

  fInstitute[idx].addInstrument(instrument);

  return 0;
}

//--------------------------------------------------------------------------
/**
 *
 */
PTheoTemplate PAdmin::getTheoTemplate(int idx)
{
  PTheoTemplate theoTemplate;

  if ((idx < 0) || (idx >= fTheoTemplate.size()))
    return theoTemplate;

  return fTheoTemplate[idx];
}

//--------------------------------------------------------------------------
/**
 *
 */
PMusrfitFunc PAdmin::getMusrfitFunc(QString name)
{
  PMusrfitFunc func;

  int idx=-1;
  for (int i=0; i<fMusrfitFunc.size(); i++) {
    if ((fMusrfitFunc[i].getName() == name) || (fMusrfitFunc[i].getAbbrv() == name)) {
      idx = i;
      break;
    }
  }

  if (idx != -1)
    func = fMusrfitFunc[idx];

  return func;
}

//--------------------------------------------------------------------------
/**
 * <p>dump all the instrument defs to stdout. This routine is there for debug
 * purposes only.
 */
void PAdmin::dump(int tag)
{
  if (tag == -1)
    return;

  if (tag != 1) {
    QVector<PInstrument> instrument;
    QVector<PSetup> setup;
    PDetector *detec;
    QVector<int> forward;
    QString forwardStr;
    QVector<int> backward;
    QString backwardStr;

    for (int i=0; i<fInstitute.size(); i++) {
      qInfo() << "debug> +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-";
      qInfo() << "debug> Institute: " << fInstitute[i].getName();
      qInfo() << "debug> ----";
      instrument = fInstitute[i].getInstruments();
      for (int j=0; j<instrument.size(); j++) {
        qInfo() << "debug> ----";
        qInfo() << "debug> Instrument Name: " << instrument[j].getName();
        // all TF setups
        setup = instrument[j].getTFSetups();
        for (int k=0; k<setup.size(); k++) {
          qInfo() << "debug> ----";
          qInfo() << "debug> TF setup, name: " << setup[k].getName();
          qInfo() << "debug>   No of Detectors: " << setup[k].getNoOfDetectors();
          qInfo() << "debug>   fgb Offset: " << setup[k].getFgbOffset();
          qInfo() << "debug>   lgb: " << setup[k].getLgb();
          qInfo() << "debug>   asymmetry bkg range: " << setup[k].getBkgStartBin() << " " << setup[k].getBkgEndBin();
          qInfo() << "debug>   ----";
          for (int l=0; l<setup[k].getNoOfLogicalDetectors(); l++) {
            detec = setup[k].getDetector(l);
            if (detec == 0) {
              qInfo() << "debug> detec == 0 for k=" << k << ", l=" << l;
              return;
            }
            forward = detec->getForwards();
            forwardStr = "";
            for (int m=0; m<forward.size(); m++)
              forwardStr += QString("%1;").arg(forward[m]);
            qInfo() << "debug>   detector : " << detec->getName() << ", " << detec->getRelGeomPhase() << "°, " << forwardStr;
          }
          qInfo() << "debug>   ----";
          for (int l=0; l<setup[k].getNoOfLogicalAsymDetectors(); l++) {
            detec = setup[k].getAsymDetector(l);
            if (detec == 0) {
              qInfo() << "debug> detec == 0 for k=" << k << ", l=" << l;
              return;
            }
            forward = detec->getForwards();
            forwardStr = "";
            backward = detec->getBackwards();
            backwardStr = "";
            for (int m=0; m<forward.size(); m++) {
              forwardStr += QString("%1;").arg(forward[m]);
              backwardStr += QString("%1;").arg(backward[m]);
            }
            qInfo() << "debug>   detectorAsym : " << detec->getName() << ", " << detec->getRelGeomPhase() << "°, " << forwardStr << "/" << backwardStr << ", alpha=" << detec->getAlpha();
          }
        }
        // all ZF setups
        setup.clear();
        setup = instrument[j].getZFSetups();
        for (int k=0; k<setup.size(); k++) {
          qInfo() << "debug> ----";
          qInfo() << "debug> ZF setup, name: " << setup[k].getName();
          qInfo() << "debug>   No of Detectors: " << setup[k].getNoOfDetectors();
          qInfo() << "debug>   fgb Offset: " << setup[k].getFgbOffset();
          qInfo() << "debug>   lgb: " << setup[k].getLgb();
          qInfo() << "debug>   asymmetry bkg range: " << setup[k].getBkgStartBin() << " " << setup[k].getBkgEndBin();
          qInfo() << "debug>   ----";
          for (int l=0; l<setup[k].getNoOfLogicalAsymDetectors(); l++) {
            detec = setup[k].getAsymDetector(l);
            if (detec == 0) {
              qInfo() << "debug> detec == 0 for k=" << k << ", l=" << l;
              return;
            }
            forward = detec->getForwards();
            forwardStr = "";
            backward = detec->getBackwards();
            backwardStr = "";
            for (int m=0; m<forward.size(); m++) {
              forwardStr += QString("%1;").arg(forward[m]);
              backwardStr += QString("%1;").arg(backward[m]);
            }
            qInfo() << "debug>   detectorAsym : " << detec->getName() << ", " << detec->getRelGeomPhase() << "°, " << forwardStr << "/" << backwardStr << ", alpha=" << detec->getAlpha();
          }
        }
        // all LF setups
        setup.clear();
        setup = instrument[j].getLFSetups();
        for (int k=0; k<setup.size(); k++) {
          qInfo() << "debug> ----";
          qInfo() << "debug> LF setup, name: " << setup[k].getName();
          qInfo() << "debug>   No of Detectors: " << setup[k].getNoOfDetectors();
          qInfo() << "debug>   fgb Offset: " << setup[k].getFgbOffset();
          qInfo() << "debug>   lgb: " << setup[k].getLgb();
          qInfo() << "debug>   asymmetry bkg range: " << setup[k].getBkgStartBin() << " " << setup[k].getBkgEndBin();
          qInfo() << "debug>   ----";
          for (int l=0; l<setup[k].getNoOfLogicalAsymDetectors(); l++) {
            detec = setup[k].getAsymDetector(l);
            if (detec == 0) {
              qInfo() << "debug> detec == 0 for k=" << k << ", l=" << l;
              return;
            }
            forward = detec->getForwards();
            forwardStr = "";
            backward = detec->getBackwards();
            backwardStr = "";
            for (int m=0; m<forward.size(); m++) {
              forwardStr += QString("%1;").arg(forward[m]);
              backwardStr += QString("%1;").arg(backward[m]);
            }
            qInfo() << "debug>   detectorAsym : " << detec->getName() << ", " << detec->getRelGeomPhase() << "°, " << forwardStr << "/" << backwardStr << ", alpha=" << detec->getAlpha();
          }
        }
      }
      qInfo() << "debug> +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-";
    }
  } else if (tag != 0) {
    qInfo() << "debug> +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-";
    qInfo() << "debug> Available Musrfit Funcs of the Wizard";
    qInfo() << "debug> +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-";
    PMusrfitFunc fun;
    for (int i=0; i<fMusrfitFunc.size(); i++) {
      fun = fMusrfitFunc[i];
      qInfo() << "debug> " << i << ": " << fun.getName() << " / " << fun.getAbbrv();
      qInfo() << "deubg> " << i << ": no of Parameters: " << fun.getNoOfParam();
      for (int j=0; j<fun.getFuncParams()->size(); j++) {
        qInfo() << "debug> " << i << "/" << j << ": param name: " << fun.getFuncParam(j).getParamName() << ", val: " << fun.getFuncParam(j).getParamValue();
      }
      qInfo() << "debug> ----";
    }
    qInfo() << "debug> +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-";
  }
}

//--------------------------------------------------------------------------
/**
 *
 */
PInstrument *PAdmin::getInstrument(QString institute, QString instrument)
{
  for (int i=0; i<fInstitute.size(); i++) {
    if (!fInstitute[i].getName().compare(institute, Qt::CaseInsensitive))
      return fInstitute[i].getInstrument(instrument);
  }

  return 0;
}

//--------------------------------------------------------------------------
/**
 *
 */
int PAdmin::loadMusrWizDefault(QString fln)
{
  // check if fln already exists
  if (!QFile::exists(fln)) {
    QFile file(":/musrWiz.xml");
    if (file.exists()) {
      // get $HOME
      QProcessEnvironment procEnv = QProcessEnvironment::systemEnvironment();
      QString pathName = procEnv.value("HOME", "");
      pathName += "/.musrfit/musrWiz";
      // check if the directory $HOME/.musrfit/musrWiz exists if not create it
      QDir dir(pathName);
      if (!dir.exists()) {
        // directory $HOME/.musrfit/musredit does not exist hence create it
        dir.mkpath(pathName);
      }
      pathName += "/musrWiz.xml";
      if (file.copy(pathName)) {
        cout << "**INFO** '" << pathName.toLatin1().data() << "' did not exist, hence it is created." << endl;
      } else {
        QString msg = QString("Couldn't create '%1'. Please check.").arg(pathName);
        QMessageBox::critical(nullptr, "ERROR", msg);
        return 1;
      }
    }
  }

  PMusrWizDefaultXMLParser handler(fln, this);
  if (!handler.isValid()) {
    QString errMsg = QString("Error parsing %1 file.").arg(fln);
    QMessageBox::critical(nullptr, "ERROR", errMsg);
    return 1;
  }
  return 0;
}

//--------------------------------------------------------------------------
/**
 *
 */
int PAdmin::loadMusrfitFunc(QString fln)
{
  // check if fln already exists
  if (!QFile::exists(fln)) {
    QFile file(":/func_defs/musrfit_funcs.xml");
    if (file.exists()) {
      // get $HOME
      QProcessEnvironment procEnv = QProcessEnvironment::systemEnvironment();
      QString pathName = procEnv.value("HOME", "");
      pathName += "/.musrfit/musrWiz/musrfit_funcs.xml";
      if (file.copy(pathName)) {
        cout << "**INFO** '" << pathName.toLatin1().data() << "' did not exist, hence it is created." << endl;
      } else {
        QString msg = QString("Couldn't create '%1'. Please check.").arg(pathName);
        QMessageBox::critical(nullptr, "ERROR", msg);
        return 1;
      }
    }
  }

  PFuncXMLParser handler(fln, this);
  if (!handler.isValid()) {
    QString errMsg = QString("Error parsing %1 musrfit func file.").arg(fln);
    QMessageBox::critical(nullptr, "ERROR", errMsg);
    return 1;
  }
  return 0;
}

//--------------------------------------------------------------------------
/**
 * @brief PAdmin::loadInstrumentDef
 * @param path
 * @param fln
 * @return
 */
int PAdmin::loadInstrumentDef(QString path, QString fln)
{
  // check if fln already exists
  QString pathFln = path+fln;
  if (!QFile::exists(pathFln)) {
    QString resFln = QString(":/instrument_defs/%1").arg(fln);
    QFile file(resFln);
    if (file.exists()) {
      // get $HOME
      QProcessEnvironment procEnv = QProcessEnvironment::systemEnvironment();
      QString pathName = procEnv.value("HOME", "");
      pathName += "/.musrfit/musrWiz/" + fln;
      if (file.copy(pathName)) {
        cout << "**INFO** '" << pathName.toLatin1().data() << "' did not exist, hence it is created." << endl;
      } else {
        QString msg = QString("Couldn't create '%1'. Please check.").arg(pathName);
        QMessageBox::critical(nullptr, "ERROR", msg);
        return 1;
      }
    } else {
      return 0;
    }
  }

  PInstrumentDefXMLParser handler(pathFln, this);
  if (!handler.isValid()) {
    QString errMsg = QString("Error parsing %1 instrument def file.").arg(pathFln);
    QMessageBox::critical(nullptr, "ERROR", errMsg);
    return 1;
  }

  return 0;
}
