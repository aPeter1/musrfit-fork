/****************************************************************************

  PAdmin.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

  $Id$

*****************************************************************************/

/***************************************************************************
 *   Copyright (C) 2010 by Andreas Suter                                   *
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

#include <QMessageBox>

#include "PAdmin.h"

//--------------------------------------------------------------------------
// implementation of PAdminXMLParser class
//--------------------------------------------------------------------------
/**
 * <p>XML Parser class for the musredit administration file.
 *
 * \param admin pointer to an admin class instance.
 */
PAdminXMLParser::PAdminXMLParser(PAdmin *admin) : fAdmin(admin)
{
  fKeyWord = eEmpty;
  fFunc = false;
}

//--------------------------------------------------------------------------
/**
 * <p>Routine called at the beginning of the XML parsing process.
 */
bool PAdminXMLParser::startDocument()
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
bool PAdminXMLParser::startElement( const QString&, const QString&,
                                       const QString& qName,
                                       const QXmlAttributes& )
{
  if (qName == "font_name") {
    fKeyWord = eFontName;
  } else if (qName == "font_size") {
    fKeyWord = eFontSize;
  } else if (qName == "exec_path") {
    fKeyWord = eExecPath;
  } else if (qName == "default_save_path") {
    fKeyWord = eDefaultSavePath;
  } else if (qName == "title_from_data_file") {
    fKeyWord = eTitleFromDataFile;
  } else if (qName == "enable_musrt0") {
    fKeyWord = eEnableMusrT0;
  } else if (qName == "beamline") {
    fKeyWord = eBeamline;
  } else if (qName == "institute") {
    fKeyWord = eInstitute;
  } else if (qName == "file_format") {
    fKeyWord = eFileFormat;
  } else if (qName == "lifetime_correction") {
    fKeyWord = eLifetimeCorrection;
  } else if (qName == "msr_default_file_path") {
    fKeyWord = eMsrDefaultFilePath;
  } else if (qName == "musr_web_main") {
    fKeyWord = eHelpMain;
  } else if (qName == "musr_web_title") {
    fKeyWord = eHelpTitle;
  } else if (qName == "musr_web_parameters") {
    fKeyWord = eHelpParameters;
  } else if (qName == "musr_web_theory") {
    fKeyWord = eHelpTheory;
  } else if (qName == "musr_web_functions") {
    fKeyWord = eHelpFunctions;
  } else if (qName == "musr_web_run") {
    fKeyWord = eHelpRun;
  } else if (qName == "musr_web_command") {
    fKeyWord = eHelpCommand;
  } else if (qName == "musr_web_fourier") {
    fKeyWord = eHelpFourier;
  } else if (qName == "musr_web_plot") {
    fKeyWord = eHelpPlot;
  } else if (qName == "musr_web_statistic") {
    fKeyWord = eHelpStatistic;
  } else if (qName == "musr_web_msr2data") {
    fKeyWord = eHelpMsr2Data;    
  } else if (qName == "chain_fit") {
    fKeyWord = eChainFit;
  } else if (qName == "write_data_header") {
    fKeyWord = eWriteDataHeader;
  } else if (qName == "summary_files_present") {
    fKeyWord = eSummaryFilesPresent;
  } else if (qName == "keep_minuit2_output") {
    fKeyWord = eKeepMinuit2Output;
  } else if (qName == "write_column_data") {
    fKeyWord = eWriteColumnData;
  } else if (qName == "recreate_data_file") {
    fKeyWord = eRecreateDataFile;
  } else if (qName == "open_file_after_fitting") {
    fKeyWord = eOpenFileAfterFitting;
  } else if (qName == "create_msr_file_only") {
    fKeyWord = eCreateMsrFileOnly;
  } else if (qName == "fit_only") {
    fKeyWord = eFitOnly;
  } else if (qName == "global") {
    fKeyWord = eGlobal;
  } else if (qName == "func_pixmap_path") {
    fKeyWord = eTheoFuncPixmapPath;
  } else if (qName == "func") {
    fKeyWord = eFunc;
    fFunc = true;
    // init theory item
    fTheoryItem.name = "";
    fTheoryItem.comment = "";
    fTheoryItem.label = "";
    fTheoryItem.pixmapName = "";
    fTheoryItem.pixmap = 0;
    fTheoryItem.params = -1;
  } else if (qName == "name") {
    fKeyWord = eFuncName;
  } else if (qName == "comment") {
    fKeyWord = eFuncComment;
  } else if (qName == "label") {
    fKeyWord = eFuncLabel;
  } else if (qName == "pixmap") {
    fKeyWord = eFuncPixmap;
  } else if (qName == "params") {
    fKeyWord = eFuncParams;
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
bool PAdminXMLParser::endElement( const QString&, const QString&, const QString &qName )
{
  fKeyWord = eEmpty;

  if (qName == "func") {
    fFunc = false;
    fAdmin->addTheoryItem(fTheoryItem);
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
bool PAdminXMLParser::characters(const QString& str)
{
  QString help;
  bool flag, ok;
  int  ival;

  switch (fKeyWord) {
    case eFontName:
      fAdmin->setFontName(QString(str.toLatin1()).trimmed());
      break;
    case eFontSize:
      ival = QString(str.toLatin1()).trimmed().toInt(&ok);
      if (ok)
        fAdmin->setFontSize(ival);
      break;
    case eExecPath:
      fAdmin->setExecPath(QString(str.toLatin1()).trimmed());
      break;
    case eDefaultSavePath:
      fAdmin->setDefaultSavePath(QString(str.toLatin1()).trimmed());
      break;
    case eTitleFromDataFile:
      if (str == "y")
        flag = true;
      else
        flag = false;
      fAdmin->setTitleFromDataFileFlag(flag);
      break;
    case eEnableMusrT0:
      if (str == "y")
        flag = true;
      else
        flag = false;
      fAdmin->setEnableMusrT0Flag(flag);
      break;
    case eBeamline:
      fAdmin->setBeamline(QString(str.toLatin1()).trimmed());
      break;
    case eInstitute:
      fAdmin->setInstitute(QString(str.toLatin1()).trimmed());
      break;
    case eFileFormat:
      fAdmin->setFileFormat(QString(str.toLatin1()).trimmed());
      break;
    case eLifetimeCorrection:
      if (str == "y")
        flag = true;
      else
        flag = false;
      fAdmin->setLifetimeCorrectionFlag(flag);
      break;
    case eMsrDefaultFilePath:
      fAdmin->setMsrDefaultFilePath(QString(str.toLatin1()).trimmed());
      break;
    case eHelpMain:
      fAdmin->setHelpUrl("main", str);
      break;
    case eHelpTitle:
      fAdmin->setHelpUrl("title", str);
      break;
    case eHelpParameters:
      fAdmin->setHelpUrl("parameters", str);
      break;
    case eHelpTheory:
      fAdmin->setHelpUrl("theory", str);
      break;
    case eHelpFunctions:
      fAdmin->setHelpUrl("functions", str);
      break;
    case eHelpRun:
      fAdmin->setHelpUrl("run", str);
      break;
    case eHelpCommand:
      fAdmin->setHelpUrl("command", str);
      break;
    case eHelpFourier:
      fAdmin->setHelpUrl("fourier", str);
      break;
    case eHelpPlot:
      fAdmin->setHelpUrl("plot", str);
      break;
    case eHelpStatistic:
      fAdmin->setHelpUrl("statistic", str);
      break;
    case eHelpMsr2Data:
      fAdmin->setHelpUrl("msr2data", str);
      break;
    case eChainFit:
      if (str == "y")
        flag = true;
      else
        flag = false;
      fAdmin->fMsr2DataParam.chainFit = flag;
      break;
    case eWriteDataHeader:
      if (str == "y")
        flag = true;
      else
        flag = false;
      fAdmin->fMsr2DataParam.writeDbHeader = flag;
      break;
    case eSummaryFilesPresent:
      if (str == "y")
        flag = true;
      else
        flag = false;
      fAdmin->fMsr2DataParam.summaryFilePresent = flag;
      break;
    case eKeepMinuit2Output:
      if (str == "y")
        flag = true;
      else
        flag = false;
      fAdmin->fMsr2DataParam.keepMinuit2Output = flag;
      break;
    case eWriteColumnData:
      if (str == "y")
        flag = true;
      else
        flag = false;
      fAdmin->fMsr2DataParam.writeColumnData = flag;
      break;
    case eRecreateDataFile:
      if (str == "y")
        flag = true;
      else
        flag = false;
      fAdmin->fMsr2DataParam.recreateDbFile = flag;
      break;
    case eOpenFileAfterFitting:
      if (str == "y")
        flag = true;
      else
        flag = false;
      fAdmin->fMsr2DataParam.openFilesAfterFitting = flag;
      break;
    case eCreateMsrFileOnly:
      if (str == "y")
        flag = true;
      else
        flag = false;
      fAdmin->fMsr2DataParam.createMsrFileOnly = flag;
      break;
    case eFitOnly:
      if (str == "y")
        flag = true;
      else
        flag = false;
      fAdmin->fMsr2DataParam.fitOnly = flag;
      break;
    case eGlobal:
      if (str == "y")
        flag = true;
      else
        flag = false;
      fAdmin->fMsr2DataParam.global = flag;
      break;
    case eTheoFuncPixmapPath:
      fAdmin->setTheoFuncPixmapPath(QString(str.toLatin1()).trimmed());
      break;
    default:
      break;
  }

  if (fFunc) {
    bool ok;
    switch (fKeyWord) {
      case eFuncName:
        fTheoryItem.name = QString(str.toLatin1()).trimmed();
        break;
      case eFuncComment:
        fTheoryItem.comment = QString(str.toLatin1()).trimmed();
        break;
      case eFuncLabel:
        fTheoryItem.label = QString(str.toLatin1()).trimmed();
        break;
      case eFuncPixmap:
        fTheoryItem.pixmapName = QString(str.toLatin1()).trimmed();
        break;
      case eFuncParams:
        fTheoryItem.params = str.toInt(&ok);
        if (!ok)
          return false;
        break;
      default:
        break;
    }
  }

  return true;
}

//--------------------------------------------------------------------------
/**
 * <p>Called at the end of the XML parse process. It checks if default paths
 * contain system variables, and if so expand them for the further use.
 */
bool PAdminXMLParser::endDocument()
{
  // check if all necessary items are found
  QString str;

  if (fAdmin->getExecPath().indexOf('$') >= 0) {
    str = expandPath(fAdmin->getExecPath());
    if (!str.isEmpty())
      fAdmin->setExecPath(str);
  }

  if (fAdmin->getDefaultSavePath().indexOf('$') >= 0) {
    str = expandPath(fAdmin->getDefaultSavePath());
    if (!str.isEmpty())
      fAdmin->setDefaultSavePath(str);
  }

  if (fAdmin->getMsrDefaultFilePath().indexOf('$') >= 0) {
    str = expandPath(fAdmin->getMsrDefaultFilePath());
    if (!str.isEmpty())
      fAdmin->setMsrDefaultFilePath(str);
  }

  if (fAdmin->getTheoFuncPixmapPath().indexOf('$') >=0) {
    str = expandPath(fAdmin->getTheoFuncPixmapPath());
    if (!str.isEmpty())
      fAdmin->setTheoFuncPixmapPath(str);
  }

  return true;
}

//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param exception
 */
bool PAdminXMLParser::warning( const QXmlParseException & exception )
{
  QString msg;

  msg  = QString("**WARNING** while parsing musredit_startup.xml in line no %1\n").arg(exception.lineNumber());
  msg += QString("**WARNING MESSAGE** ") + exception.message();

  qWarning() << endl << msg << endl;

  QMessageBox::warning(0, "WARNING", msg, QMessageBox::Ok, QMessageBox::NoButton);

  return true;
}

//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param exception
 */
bool PAdminXMLParser::error( const QXmlParseException & exception )
{
  QString msg;

  msg  = QString("**ERROR** while parsing musredit_startup.xml in line no %1\n").arg(exception.lineNumber());
  msg += QString("**ERROR MESSAGE** ") + exception.message();

  qWarning() << endl << msg << endl;

  QMessageBox::critical(0, "ERROR", msg, QMessageBox::Ok, QMessageBox::NoButton);

  return true;
}

//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param exception
 */
bool PAdminXMLParser::fatalError( const QXmlParseException & exception )
{
  QString msg;

  msg  = QString("**FATAL ERROR** while parsing musredit_startup.xml in line no %1\n").arg(exception.lineNumber());
  msg += QString("**FATAL ERROR MESSAGE** ") + exception.message();

  qWarning() << endl << msg << endl;

  QMessageBox::critical(0, "FATAL ERROR", msg, QMessageBox::Ok, QMessageBox::NoButton);

  return true;
}

//--------------------------------------------------------------------------
/**
 * <p>Expands system variables to full path, e.g. <tt>$HOME -> \home\user</tt>
 *
 * \param str path string with none expanded system variables.
 */
QString PAdminXMLParser::expandPath(const QString &str)
{
  QString token;
  QString path;
  QString msg;
  QString newStr="";

  QStringList list = str.split("/");

  for ( QStringList::Iterator it = list.begin(); it != list.end(); ++it ) {
    token = *it;
    if (token.contains("$")) {
      token.remove('$');
      path = std::getenv(token.toLatin1());
      if (path.isEmpty()) {
        msg = QString("Couldn't expand '%1'. Some things might not work properly").arg(token);
        QMessageBox::warning(0, "**WARNING**", msg, QMessageBox::Ok, QMessageBox::NoButton);
        newStr = "";
        break;
      }
      newStr += path;
    } else {
      newStr += "/" + token;
    }
  }

  return newStr;
}

//--------------------------------------------------------------------------
// implementation of PAdmin class
//--------------------------------------------------------------------------
/**
 * <p>Initializes that PAdmin object, and calls the XML parser which feeds
 * the object variables.
 */
PAdmin::PAdmin()
{
  fFontName = QString("Courier"); // default font
  fFontSize = 11; // default font size

  fExecPath = QString("");
  fDefaultSavePath = QString("");
  fMsrDefaultFilePath = QString("");
  fTheoFuncPixmapPath = QString("");

  fBeamline   = QString("");
  fInstitute  = QString("");
  fFileFormat = QString("");

  fTitleFromDataFile  = false;
  fEnableMusrT0       = false;
  fLifetimeCorrection = true;

  fMsr2DataParam.firstRun = -1;
  fMsr2DataParam.lastRun  = -1;
  fMsr2DataParam.runList  = QString("");
  fMsr2DataParam.runListFileName = QString("");
  fMsr2DataParam.msrFileExtension = QString("");
  fMsr2DataParam.templateRunNo = -1;
  fMsr2DataParam.dbOutputFileName = QString("");
  fMsr2DataParam.writeDbHeader = true;
  fMsr2DataParam.summaryFilePresent = true;
  fMsr2DataParam.keepMinuit2Output = false;
  fMsr2DataParam.writeColumnData = false;
  fMsr2DataParam.recreateDbFile = false;
  fMsr2DataParam.chainFit = true;
  fMsr2DataParam.openFilesAfterFitting = true;
  fMsr2DataParam.titleFromDataFile = true;
  fMsr2DataParam.createMsrFileOnly = false;
  fMsr2DataParam.fitOnly = false;
  fMsr2DataParam.global = false;

  // XML Parser part
  QString fln = "./musredit_startup.xml";
  if (!QFile::exists(fln)) {
    QString path = std::getenv("MUSRFITPATH");
    QString rootsys = std::getenv("ROOTSYS");
    if (path.isEmpty())
      path = rootsys + "/bin";
    fln = path + "/musredit_startup.xml";
  }
  if (QFile::exists(fln)) { // administration file present
    PAdminXMLParser handler(this);
    QFile xmlFile(fln);
    QXmlInputSource source( &xmlFile );
    QXmlSimpleReader reader;
    reader.setContentHandler( &handler );
    reader.setErrorHandler( &handler );
    if (!reader.parse( source )) {
      QMessageBox::critical(0, "ERROR",
                            "Error parsing musredit_startup.xml settings file.\nProbably a few things will not work porperly.\nPlease fix this first.",
                            QMessageBox::Ok, QMessageBox::NoButton);
    }
  } else {
    QMessageBox::critical(0, "ERROR",
                          "Couldn't find the musredit_startup.xml settings file.\nProbably a few things will not work porperly.\nPlease fix this first.",
                          QMessageBox::Ok, QMessageBox::NoButton);
  }
}

//--------------------------------------------------------------------------
// implementation of PAdmin class
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 * <p>returns the help url corresponding the the tag.
 *
 * \param tag to map the help url. At the moment the following tags should be present:
 * main, title, parameters, theory, functions, run, command, fourier, plot, statistic
 */
QString PAdmin::getHelpUrl(QString tag)
{
  return fHelpUrl[tag];
}

//--------------------------------------------------------------------------
/**
 * <p>returns a theory item from position idx. If idx is out of the range, a null pointer is returned.
 *
 * \param idx position of the theory item.
 */
PTheory* PAdmin::getTheoryItem(const unsigned int idx)
{
  if (idx > (unsigned int)fTheory.size())
    return 0;
  else
    return &fTheory[idx];
}

//--------------------------------------------------------------------------
/**
 * <p>set the help url, addressed via a tag. At the moment the following tags should be present:
 * main, title, parameters, theory, functions, run, command, fourier, plot, statistic, msr2data
 *
 * \param tag to address the help url
 * \param url help url corresponding to the tag.
 */
void PAdmin::setHelpUrl(const QString tag, const QString url)
{
  fHelpUrl[tag] = url;
}

//--------------------------------------------------------------------------
// END
//--------------------------------------------------------------------------
