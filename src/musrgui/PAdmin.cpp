/****************************************************************************

  PAdmin.cpp

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

#include <cstdlib>

#include <qmessagebox.h>

#include "PAdmin.h"

//--------------------------------------------------------------------------
// implementation of PAdminXMLParser class
//--------------------------------------------------------------------------
/**
 * <p>XML Parser class for the musrgui administration file.
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
  } else if (qName == "help_main") {
    fKeyWord = eHelpMain;
  } else if (qName == "chain_fit") {
    fKeyWord = eChainFit;
  } else if (qName == "write_data_header") {
    fKeyWord = eWriteDataHeader;
  } else if (qName == "ignore_data_header_info") {
    fKeyWord = eIgnoreDataHeaderInfo;
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
  } else if (qName == "global_plus") {
    fKeyWord = eGlobalPlus;
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
 * put the filtering tag to 'empty'.
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
 */
bool PAdminXMLParser::characters(const QString& str)
{
  QString help;
  bool flag, ok;
  int  ival;

  switch (fKeyWord) {
    case eFontName:
      fAdmin->setFontName(QString(str.ascii()).stripWhiteSpace());
      break;
    case eFontSize:
      ival = QString(str.ascii()).stripWhiteSpace().toInt(&ok);
      if (ok)
        fAdmin->setFontSize(ival);
      break;
    case eExecPath:
      fAdmin->setExecPath(QString(str.ascii()).stripWhiteSpace());
      break;
    case eDefaultSavePath:
      fAdmin->setDefaultSavePath(QString(str.ascii()).stripWhiteSpace());
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
      fAdmin->setBeamline(QString(str.ascii()).stripWhiteSpace());
      break;
    case eInstitute:
      fAdmin->setInstitute(QString(str.ascii()).stripWhiteSpace());
      break;
    case eFileFormat:
      fAdmin->setFileFormat(QString(str.ascii()).stripWhiteSpace());
      break;
    case eLifetimeCorrection:
      if (str == "y")
        flag = true;
      else
        flag = false;
      fAdmin->setLifetimeCorrectionFlag(flag);
      break;
    case eMsrDefaultFilePath:
      fAdmin->setMsrDefaultFilePath(QString(str.ascii()).stripWhiteSpace());
      break;
    case eHelpMain:
      help = str;
      help.replace("&gt;", ">");
      help.replace("&lt;", "<");
      fAdmin->setHelpMain(help);
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
    case eIgnoreDataHeaderInfo:
      if (str == "y")
        flag = true;
      else
        flag = false;
      fAdmin->fMsr2DataParam.ignoreDataHeaderInfo = flag;
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
    case eGlobalPlus:
      if (str == "y")
        flag = true;
      else
        flag = false;
      fAdmin->fMsr2DataParam.globalPlus = flag;
    break;
    case eTheoFuncPixmapPath:
      fAdmin->setTheoFuncPixmapPath(QString(str.ascii()).stripWhiteSpace());
      break;
    default:
      break;
  }

  if (fFunc) {
    bool ok;
    switch (fKeyWord) {
      case eFuncName:
        fTheoryItem.name = QString(str.latin1()).stripWhiteSpace();
        break;
      case eFuncComment:
        fTheoryItem.comment = QString(str.latin1()).stripWhiteSpace();
        break;
      case eFuncLabel:
        fTheoryItem.label = QString(str.latin1()).stripWhiteSpace();
        break;
      case eFuncPixmap:
        fTheoryItem.pixmapName = QString(str.latin1()).stripWhiteSpace();
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
 * <p>Called at the end of the XML parse process.
 */
bool PAdminXMLParser::endDocument()
{
  // check if all necessary items are found
  QString str;

  if (fAdmin->getExecPath().find('$') >= 0) {
    str = expandPath(fAdmin->getExecPath());
    if (!str.isEmpty())
      fAdmin->setExecPath(str);
  }

  if (fAdmin->getDefaultSavePath().find('$') >= 0) {
    str = expandPath(fAdmin->getDefaultSavePath());
    if (!str.isEmpty())
      fAdmin->setDefaultSavePath(str);
  }

  if (fAdmin->getMsrDefaultFilePath().find('$') >= 0) {
    str = expandPath(fAdmin->getMsrDefaultFilePath());
    if (!str.isEmpty())
      fAdmin->setMsrDefaultFilePath(str);
  }

  if (fAdmin->getTheoFuncPixmapPath().find('$') >=0) {
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
  msg += QString("**WARNING MESSAGE** ") + exception.message() + QString("\n");

  qWarning( msg.latin1() );

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
  msg += QString("**ERROR MESSAGE** ") + exception.message() + QString("\n");

  qWarning( msg.latin1() );

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
  msg += QString("**FATAL ERROR MESSAGE** ") + exception.message() + QString("\n");

  qWarning( msg.latin1() );

  QMessageBox::critical(0, "FATAL ERROR", msg, QMessageBox::Ok, QMessageBox::NoButton);

  return true;
}

//--------------------------------------------------------------------------
/**
 * <p>Called at the end of the XML parse process.
 */
QString PAdminXMLParser::expandPath(const QString &str)
{
  QString token;
  QString path;
  QString msg;
  QString newStr="";

  QStringList list = QStringList::split("/", str);

  for ( QStringList::Iterator it = list.begin(); it != list.end(); ++it ) {
    token = *it;
    if (token.contains("$")) {
      token.remove('$');
      path = std::getenv(token.latin1());
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
 * <p>
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

  fHelpMain   = QString("");

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
  fMsr2DataParam.ignoreDataHeaderInfo = false;
  fMsr2DataParam.keepMinuit2Output = false;
  fMsr2DataParam.writeColumnData = false;
  fMsr2DataParam.recreateDbFile = false;
  fMsr2DataParam.chainFit = true;
  fMsr2DataParam.openFilesAfterFitting = true;
  fMsr2DataParam.titleFromDataFile = true;
  fMsr2DataParam.createMsrFileOnly = false;
  fMsr2DataParam.fitOnly = false;
  fMsr2DataParam.global = false;
  fMsr2DataParam.globalPlus = false;

  // XML Parser part
  QString fln = "./musrgui_startup.xml";
  if (!QFile::exists(fln)) {
    QString path = std::getenv("MUSRFITPATH");
    QString rootsys = std::getenv("ROOTSYS");
    if (path.isEmpty())
      path = rootsys + "/bin";
    fln = path + "/musrgui_startup.xml";
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
                          "Couldn't find the musrgui_startup.xml settings file.\nProbably a few things will not work porperly.\nPlease fix this first.",
                          QMessageBox::Ok, QMessageBox::NoButton);
  }
}

//--------------------------------------------------------------------------
// implementation of PAdmin class
//--------------------------------------------------------------------------
/**
 * <p>
 */
PTheory* PAdmin::getTheoryItem(const unsigned int idx)
{
  if (idx > fTheory.size())
    return 0;
  else
    return &fTheory[idx];
}

//--------------------------------------------------------------------------
// END
//--------------------------------------------------------------------------
