/****************************************************************************

  PAdmin.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

*****************************************************************************/

/***************************************************************************
 *   Copyright (C) 2010-2016 by Andreas Suter                              *
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
#include <QFile>
#include <QTextStream>
#include <QVector>

#include "PAdmin.h"

//--------------------------------------------------------------------------
// implementation of PFuncXMLParser class
//--------------------------------------------------------------------------
/**
 * <p>XML Parser class for the musrWiz function file.
 *
 * \param admin pointer to an admin class instance.
 */
PFuncXMLParser::PFuncXMLParser(PAdmin *admin) : fAdmin(admin)
{
  fKeyWord = eEmpty;
}

//--------------------------------------------------------------------------
/**
 * <p>
 */
bool PFuncXMLParser::startDocument()
{
  return true;
}

//--------------------------------------------------------------------------
/**
 * <p>
 */
bool PFuncXMLParser::startElement(const QString&, const QString&,
                                  const QString& qName,
                                  const QXmlAttributes& qAttr)
{
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
    if (qAttr.count() != 2) {
      errMsg = QString("theo_map should have 2 attributes, called 'no', and 'name', found %1").arg(qAttr.count());
      QMessageBox::critical(0, "ERROR", errMsg);
      return false;
    }
    PParam map;
    for (int i=0; i<qAttr.count(); i++) {
      if (qAttr.qName(i) == "no") {
        ival = qAttr.value(i).toInt(&ok);
        if (!ok) {
          errMsg = QString("theo_map attribute 'no' is not a number (%1)").arg(qAttr.value(i));
          QMessageBox::critical(0, "ERROR", errMsg);
          return false;
        }
        map.setNumber(ival);
      } else if (qAttr.qName(i) == "name") {
        map.setName(qAttr.value(i));
      } else {
        errMsg = QString("found unkown theo_map attribute (%1)").arg(qAttr.qName(i));
        QMessageBox::critical(0, "ERROR", errMsg);
        return false;
      }
    }
    // check that all necessary attributes where found
    if ((map.getName() == "UnDef") || (map.getNumber() == -1)) {
      errMsg = QString("found theo_map with missing attribute(s)");
      QMessageBox::critical(0, "ERROR", errMsg);
      return false;
    }
    fTheoTemplate.appendMap(map);
  } else if (qName == "template_param") {
    if ((qAttr.count() != 4) && (qAttr.count() != 6)) {
      errMsg = QString("template_param should have 4 or 6 attributes, called\n'no', 'name', 'value', 'step', ['boundLow', 'boundHigh'] found %1").arg(qAttr.count());
      QMessageBox::critical(0, "ERROR", errMsg);
      return false;
    }
    PParam param;
    for (int i=0; i<qAttr.count(); i++) {
      if (qAttr.qName(i) == "no") {
        ival = qAttr.value(i).toInt(&ok);
        if (!ok) {
          errMsg = QString("template_param attribute 'no' is not a number (%1)").arg(qAttr.value(i));
          QMessageBox::critical(0, "ERROR", errMsg);
          return false;
        }
        param.setNumber(ival);
      } else if (qAttr.qName(i) == "name") {
        param.setName(qAttr.value(i));
      } else if (qAttr.qName(i) == "value") {
        dval = qAttr.value(i).toDouble(&ok);
        if (!ok) {
          errMsg = QString("template_param attribute 'value' is not a number (%1)").arg(qAttr.value(i));
          QMessageBox::critical(0, "ERROR", errMsg);
          return false;
        }
        param.setValue(dval);
      } else if (qAttr.qName(i) == "step") {
        dval = qAttr.value(i).toDouble(&ok);
        if (!ok) {
          errMsg = QString("template_param attribute 'step' is not a number (%1)").arg(qAttr.value(i));
          QMessageBox::critical(0, "ERROR", errMsg);
          return false;
        }
        param.setStep(dval);
      } else if (qAttr.qName(i) == "boundLow") {
        param.setBoundLow(qAttr.qName(i));
      } else if (qAttr.qName(i) == "boundHigh") {
        param.setBoundHigh(qAttr.qName(i));
      }
    }
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
 * <p>
 */
bool PFuncXMLParser::endElement( const QString&, const QString&, const QString &qName )
{
  fKeyWord = eEmpty;

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
 * <p>
 */
bool PFuncXMLParser::characters(const QString &str)
{
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
 * <p>
 */
bool PFuncXMLParser::endDocument()
{
  return true;
}

//--------------------------------------------------------------------------
/**
 * <p>
 */
bool PFuncXMLParser::warning( const QXmlParseException & exception )
{
  QString msg;

  msg  = QString("**WARNING** while parsing musrfit_funcs.xml in line no %1\n").arg(exception.lineNumber());
  msg += QString("**WARNING MESSAGE** ") + exception.message();

  qWarning() << endl << msg << endl;

  QMessageBox::warning(0, "WARNING", msg, QMessageBox::Ok, QMessageBox::NoButton);

  return true;
}

//--------------------------------------------------------------------------
/**
 * <p>
 */
bool PFuncXMLParser::error( const QXmlParseException & exception )
{
  QString msg;

  msg  = QString("**ERROR** while parsing musrfit_funcs.xml in line no %1\n").arg(exception.lineNumber());
  msg += QString("**ERROR MESSAGE** ") + exception.message();

  qWarning() << endl << msg << endl;

  QMessageBox::warning(0, "ERROR", msg, QMessageBox::Ok, QMessageBox::NoButton);

  return true;
}

//--------------------------------------------------------------------------
/**
 * <p>
 */
bool PFuncXMLParser::fatalError( const QXmlParseException & exception )
{
  QString msg;

  msg  = QString("**FATAL ERROR** while parsing musrfit_funcs.xml in line no %1\n").arg(exception.lineNumber());
  msg += QString("**FATAL ERROR MESSAGE** ") + exception.message();

  qWarning() << endl << msg << endl;

  QMessageBox::warning(0, "FATAL ERROR", msg, QMessageBox::Ok, QMessageBox::NoButton);

  return true;
}

//--------------------------------------------------------------------------
// implementation of PInstrumentDefXMLParser class
//--------------------------------------------------------------------------
/**
 * <p>XML Parser class for the instrument definition file(s).
 *
 * \param admin pointer to an admin class instance.
 */
PInstrumentDefXMLParser::PInstrumentDefXMLParser(PAdmin *admin) : fAdmin(admin)
{
  fKeyWord = eEmpty;

  fInstituteName = "";
  fInstrument = 0;
  fSetup = 0;
}

//--------------------------------------------------------------------------
/**
 * <p>Routine called at the beginning of the XML parsing process.
 */
bool PInstrumentDefXMLParser::startDocument()
{
  return true;
}

//--------------------------------------------------------------------------
/**
 * <p>Routine called when a new XML tag is found. Here it is used
 * to set a tag for filtering afterwards the content.
 *
 * \param qName name of the XML tag.
 */
bool PInstrumentDefXMLParser::startElement( const QString&, const QString&,
                                    const QString& qName,
                                    const QXmlAttributes& qAttr)
{
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
    if (qAttr.count() != 1) {
      errMsg = QString("instrument should have 1 attribute, called 'name', found %1").arg(qAttr.count());
      QMessageBox::critical(0, "ERROR", errMsg);
      return false;
    }
    if (fInstituteName == "") {
      errMsg = QString("found instrument without institute set.");
      QMessageBox::critical(0, "ERROR", errMsg);
      return false;
    }
    // create an instrument object
    fInstrument = new PInstrument();
    fInstrument->setInstitue(fInstituteName);
    fInstrument->setName(qAttr.value(0));
  } else if (qName == "run_name_template") {
    fKeyWord = eRunNameTemplate;
  } else if (qName == "beamline") {
    fKeyWord = eBeamline;
  } else if (qName == "data_file_format") {
    fKeyWord = eDataFileFormat;
  } else if (qName == "tf") {
    fKeyWord = eTf;
    fSetup = new PSetup();
    if (qAttr.count() == 1)
      fSetup->setName(qAttr.value(0));
    else
      fSetup->setName("Default");
  } else if (qName == "zf") {
    fKeyWord = eZf;
    fSetup = new PSetup();
    if (qAttr.count() == 1)
      fSetup->setName(qAttr.value(0));
    else
      fSetup->setName("Default");
  } else if (qName == "lf") {
    fKeyWord = eLf;
    fSetup = new PSetup();
    if (qAttr.count() == 1)
      fSetup->setName(qAttr.value(0));
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
    if (qAttr.count() < 3)
      return false;
    PDetector detect;
    int count=0;
    for (int i=0; i<qAttr.count(); i++) {
      if (qAttr.localName(i) == "name") {
        detect.setName(qAttr.value(i)); // detector name
        count++;
      } else if (qAttr.localName(i) == "rel_phase") {
        str = qAttr.value(i);
        dval = str.toDouble(&ok);
        if (ok) {
          detect.setRelGeomPhase(dval);
          count++;
        }
      } else if (qAttr.localName(i) == "forward") {
        str = qAttr.value(i);
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
        count++;
      }
    }

    if (count != 3) // i.e. didn't find all needed attributes
      return false;
    fSetup->addDetector(detect);
  } else if (qName == "logic_asym_detector") {
    if (qAttr.count() != 5)
      return false;
    PDetector detect;
    int count=0;
    for (int i=0; i<5; i++) {
      if (qAttr.localName(i) == "name") {
        detect.setName(qAttr.value(i)); // detector name
        count++;
      } else if (qAttr.localName(i) == "rel_phase") {
        str = qAttr.value(i);
        dval = str.toDouble(&ok);
        if (ok) {
          detect.setRelGeomPhase(dval);
          count++;
        }
      } else if (qAttr.localName(i) == "forward") {
        str = qAttr.value(i);
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
        count++;
      } else if (qAttr.localName(i) == "backward") {
        str = qAttr.value(i);
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
        count++;
      } else if (qAttr.localName(i) == "alpha") {
        str = qAttr.value(i);
        dval = str.toDouble(&ok);
        if (!ok)
          return false;
        detect.setAlpha(dval);
        count++;
      }
    }

    if (count != 5) // i.e. didn't find all needed attributes
      return false;
    fSetup->addAsymDetector(detect);
  }

  return true;
}

//--------------------------------------------------------------------------
/**
 * <p>Routine called when the end XML tag is found. It is used to
 * put the filtering tag to 'empty'. It also resets the fFunc flag in case
 * the entry was a theory function.
 *
 * \param qName name of the element.
 */
bool PInstrumentDefXMLParser::endElement( const QString&, const QString&, const QString &qName )
{
  fKeyWord = eEmpty;

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
 *
 * \param str keeps the content of the XML tag.
 */
bool PInstrumentDefXMLParser::characters(const QString& str)
{
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
      QMessageBox::critical(0, "ERROR", errMsg);
      return false;
    }
    ival = str.toInt(&ok);
    if (!ok) {
      errMsg = QString("Setup Error: No of Detectors = '%1', which is not an int.").arg(str);
      QMessageBox::critical(0, "ERROR", errMsg);
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
    strList = str.split(' ', QString::SkipEmptyParts);
    if (strList.size() != 2) {
      errMsg = QString("Found wrong Asymmetry background range: '%1'").arg(str);
      QMessageBox::critical(0, "ERROR", errMsg);
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
    QMessageBox::critical(0, "FATAL ERROR", "Didn't find any institute name in the instrument definitions.");
    return false;
  }

  return true;
}

//--------------------------------------------------------------------------
/**
 * <p>Report XML warnings.
 *
 * \param exception holds the information of the XML warning
 */
bool PInstrumentDefXMLParser::warning( const QXmlParseException & exception )
{
  QString msg;

  msg  = QString("**WARNING** while parsing instrument_def_XXX.xml in line no %1\n").arg(exception.lineNumber());
  msg += QString("**WARNING MESSAGE** ") + exception.message();

  qWarning() << endl << msg << endl;

  QMessageBox::warning(0, "WARNING", msg, QMessageBox::Ok, QMessageBox::NoButton);

  return true;
}

//--------------------------------------------------------------------------
/**
 * <p>Report recoverable XML errors.
 *
 * \param exception holds the information of the XML recoverable errors.
 */
bool PInstrumentDefXMLParser::error( const QXmlParseException & exception )
{
  QString msg;

  msg  = QString("**ERROR** while parsing instrument_def_XXX.xml in line no %1\n").arg(exception.lineNumber());
  msg += QString("**ERROR MESSAGE** ") + exception.message();

  qWarning() << endl << msg << endl;

  QMessageBox::critical(0, "ERROR", msg, QMessageBox::Ok, QMessageBox::NoButton);

  return true;
}

//--------------------------------------------------------------------------
/**
 * <p>Report fatal XML errors.
 *
 * \param exception holds the information of the XML fatal errors.
 */
bool PInstrumentDefXMLParser::fatalError( const QXmlParseException & exception )
{
  QString msg;

  msg  = QString("**FATAL ERROR** while parsing parsing instrument_def_XXX.xml in line no %1\n").arg(exception.lineNumber());
  msg += QString("**FATAL ERROR MESSAGE** ") + exception.message();

  qWarning() << endl << msg << endl;

  QMessageBox::critical(0, "FATAL ERROR", msg, QMessageBox::Ok, QMessageBox::NoButton);

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
PMusrWizDefaultXMLParser::PMusrWizDefaultXMLParser(PAdmin *admin) : fAdmin(admin)
{
  fKeyWord = eEmpty;
}

//--------------------------------------------------------------------------
/**
 * <p>Routine called at the beginning of the XML parsing process.
 */
bool PMusrWizDefaultXMLParser::startDocument()
{
  return true;
}

//--------------------------------------------------------------------------
/**
 * <p>Routine called when a new XML tag is found. Here it is used
 * to set a tag for filtering afterwards the content.
 *
 * \param qName name of the XML tag.
 */
bool PMusrWizDefaultXMLParser::startElement( const QString&, const QString&,
                                             const QString& qName,
                                             const QXmlAttributes& )
{
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
 *
 * \param qName name of the element.
 */
bool PMusrWizDefaultXMLParser::endElement( const QString&, const QString&, const QString& )
{
  fKeyWord = eEmpty;

  return true;
}

//--------------------------------------------------------------------------
/**
 * <p>This routine delivers the content of an XML tag. It fills the
 * content into the load data structure.
 *
 * \param str keeps the content of the XML tag.
 */
bool PMusrWizDefaultXMLParser::characters(const QString& str)
{
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
/**
 * <p>Report XML warnings.
 *
 * \param exception holds the information of the XML warning
 */
bool PMusrWizDefaultXMLParser::warning( const QXmlParseException & exception )
{
  QString msg;

  msg  = QString("**WARNING** while parsing musrWiz.xml in line no %1\n").arg(exception.lineNumber());
  msg += QString("**WARNING MESSAGE** ") + exception.message();

  qWarning() << endl << msg << endl;

  QMessageBox::warning(0, "WARNING", msg, QMessageBox::Ok, QMessageBox::NoButton);

  return true;
}

//--------------------------------------------------------------------------
/**
 * <p>Report recoverable XML errors.
 *
 * \param exception holds the information of the XML recoverable errors.
 */
bool PMusrWizDefaultXMLParser::error( const QXmlParseException & exception )
{
  QString msg;

  msg  = QString("**ERROR** while parsing musrWiz.xml in line no %1\n").arg(exception.lineNumber());
  msg += QString("**ERROR MESSAGE** ") + exception.message();

  qWarning() << endl << msg << endl;

  QMessageBox::critical(0, "ERROR", msg, QMessageBox::Ok, QMessageBox::NoButton);

  return true;
}

//--------------------------------------------------------------------------
/**
 * <p>Report fatal XML errors.
 *
 * \param exception holds the information of the XML fatal errors.
 */
bool PMusrWizDefaultXMLParser::fatalError( const QXmlParseException & exception )
{
  QString msg;

  msg  = QString("**FATAL ERROR** while parsing parsing musrWiz.xml in line no %1\n").arg(exception.lineNumber());
  msg += QString("**FATAL ERROR MESSAGE** ") + exception.message();

  qWarning() << endl << msg << endl;

  QMessageBox::critical(0, "FATAL ERROR", msg, QMessageBox::Ok, QMessageBox::NoButton);

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
      QMessageBox::warning(0, "WARNING", "Couldn't find musrWiz.xml file.");
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
      QMessageBox::critical(0, "FATAL ERROR", "Couldn't find any musrfit function definitions.");
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
    QMessageBox::critical(0, "FATAL ERROR", "Couldn't find any instrument definition.");
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
        QMessageBox::critical(0, "ERROR", msg);
        return 1;
      }
    }
  }

  PMusrWizDefaultXMLParser handler(this);
  QFile xmlFile(fln);
  QXmlInputSource source( &xmlFile );
  QXmlSimpleReader reader;
  reader.setContentHandler( &handler );
  if (!reader.parse(source)) {
    QString errMsg = QString("Error parsing %1 file.").arg(fln);
    QMessageBox::critical(0, "ERROR", errMsg);
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
        QMessageBox::critical(0, "ERROR", msg);
        return 1;
      }
    }
  }

  PFuncXMLParser handler(this);
  QFile xmlFile(fln);
  QXmlInputSource source( &xmlFile );
  QXmlSimpleReader reader;
  reader.setContentHandler( &handler );
  if (!reader.parse(source)) {
    QString errMsg = QString("Error parsing %1 musrfit func file.").arg(fln);
    QMessageBox::critical(0, "ERROR", errMsg);
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
        QMessageBox::critical(0, "ERROR", msg);
        return 1;
      }
    } else {
      return 0;
    }
  }

  PInstrumentDefXMLParser handler(this);
  QFile xmlFile(pathFln);
  QXmlInputSource source( &xmlFile );
  QXmlSimpleReader reader;
  reader.setContentHandler( &handler );
  if (!reader.parse(source)) {
    QString errMsg = QString("Error parsing %1 instrument def file.").arg(pathFln);
    QMessageBox::critical(0, "ERROR", errMsg);
    return 1;
  }

  return 0;
}
