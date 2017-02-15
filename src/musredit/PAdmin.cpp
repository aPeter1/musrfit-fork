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
#include <QDir>
#include <QProcessEnvironment>

#include "musrfit-info.h"
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
  if (qName == "timeout") {
    fKeyWord = eTimeout;
  } else if (qName == "font_name") {
    fKeyWord = eFontName;
  } else if (qName == "font_size") {
    fKeyWord = eFontSize;
  } else if (qName == "exec_path") {
    fKeyWord = eExecPath;
  } else if (qName == "default_save_path") {
    fKeyWord = eDefaultSavePath;
  } else if (qName == "title_from_data_file") {
    fKeyWord = eTitleFromDataFile;
  } else if (qName == "musrview_show_fourier") {
    fKeyWord = eMusrviewShowFourier;
  } else if (qName == "musrview_show_avg") {
    fKeyWord = eMusrviewShowAvg;
  } else if (qName == "enable_musrt0") {
    fKeyWord = eEnableMusrT0;
  } else if (qName == "keep_minuit2_output") {
    fKeyWord = eKeepMinuit2Output;
  } else if (qName == "dump_ascii") {
    fKeyWord = eDumpAscii;
  } else if (qName == "dump_root") {
    fKeyWord = eDumpRoot;
  } else if (qName == "estimate_n0") {
    fKeyWord = eEstimateN0;
  } else if (qName == "chisq_per_run_block") {
    fKeyWord = eChisqPreRunBlock;
  } else if (qName == "path_file_name") {
    fKeyWord = eRecentFile;
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
  } else if (qName == "musr_web_musrFT") {
    fKeyWord = eHelpMusrFT;
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
    case eTimeout:
      ival = QString(str.toLatin1()).trimmed().toInt(&ok);
      if (ok)
        fAdmin->setTimeout(ival);
      break;
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
    case eMusrviewShowFourier:
      if (str == "y")
        flag = true;
      else
        flag = false;
      fAdmin->setMusrviewShowFourierFlag(flag);
      break;
    case eMusrviewShowAvg:
      if (str == "y")
        flag = true;
      else
        flag = false;
      fAdmin->setMusrviewShowAvgFlag(flag);
      break;
    case eEnableMusrT0:
      if (str == "y")
        flag = true;
      else
        flag = false;
      fAdmin->setEnableMusrT0Flag(flag);
      break;
    case eKeepMinuit2Output:
      if (str == "y")
        flag = true;
      else
        flag = false;
      fAdmin->fMsr2DataParam.keepMinuit2Output = flag;
      fAdmin->setKeepMinuit2OutputFlag(flag);
      break;
    case eDumpAscii:
      if (str == "y")
        flag = true;
      else
        flag = false;
      fAdmin->setDumpAsciiFlag(flag);
      break;
    case eDumpRoot:
      if (str == "y")
        flag = true;
      else
        flag = false;
      fAdmin->setDumpRootFlag(flag);
      break;
    case eEstimateN0:
      if (str == "y")
        flag = true;
      else
        flag = false;
      fAdmin->fMsr2DataParam.estimateN0 = flag;
      fAdmin->setEstimateN0Flag(flag);
      break;
    case eChisqPreRunBlock:
      if (str == "y")
        flag = true;
      else
        flag = false;
      fAdmin->fMsr2DataParam.perRunBlockChisq = flag;
      fAdmin->setChisqPerRunBlockFlag(flag);
      break;
    case eRecentFile:
      fAdmin->addRecentFile(QString(str.toLatin1()).trimmed());
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
    case eHelpMusrFT:
      fAdmin->setHelpUrl("musrFT", str);
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
 * <p>Report XML warnings.
 *
 * \param exception holds the information of the XML warning
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
 * <p>Report recoverable XML errors.
 *
 * \param exception holds the information of the XML recoverable errors.
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
 * <p>Report fatal XML errors.
 *
 * \param exception holds the information of the XML fatal errors.
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

  QProcessEnvironment procEnv = QProcessEnvironment::systemEnvironment();

  QStringList list = str.split("/");

  for ( QStringList::Iterator it = list.begin(); it != list.end(); ++it ) {
    token = *it;
    if (token.contains("$")) {
      token.remove('$');
      if (!procEnv.contains(token)) {
        msg = QString("Couldn't find '%1'. Some things might not work properly").arg(token);
        QMessageBox::warning(0, "**WARNING**", msg, QMessageBox::Ok, QMessageBox::NoButton);
        newStr = "";
        break;
      }
      path = procEnv.value(token, "");
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
PAdmin::PAdmin() : QObject()
{
  fTimeout = 3600;

  fFontName = QString("Courier"); // default font
  fFontSize = 11; // default font size

  fPrefPathName = QString("");
  fExecPath = QString("");
  fDefaultSavePath = QString("");
  fMsrDefaultFilePath = QString("");
  fTheoFuncPixmapPath = QString("");

  fBeamline   = QString("");
  fInstitute  = QString("");
  fFileFormat = QString("");

  fMusrviewShowFourier = false;
  fMusrviewShowAvg = false;

  fTitleFromDataFile  = false;
  fEnableMusrT0       = false;
  fLifetimeCorrection = true;
  fEstimateN0         = true;
  fChisqPreRunBlock   = false;

  fMsr2DataParam.runList  = QString("");
  fMsr2DataParam.runListFileName = QString("");
  fMsr2DataParam.msrFileExtension = QString("");
  fMsr2DataParam.templateRunNo = -1;
  fMsr2DataParam.dbOutputFileName = QString("");
  fMsr2DataParam.writeDbHeader = true;
  fMsr2DataParam.ignoreDataHeaderInfo = false;
  fMsr2DataParam.keepMinuit2Output = false;
  fMsr2DataParam.estimateN0 = fEstimateN0;
  fMsr2DataParam.writeColumnData = false;
  fMsr2DataParam.recreateDbFile = false;
  fMsr2DataParam.chainFit = true;
  fMsr2DataParam.openFilesAfterFitting = true;
  fMsr2DataParam.perRunBlockChisq = fChisqPreRunBlock;
  fMsr2DataParam.titleFromDataFile = true;
  fMsr2DataParam.createMsrFileOnly = false;
  fMsr2DataParam.fitOnly = false;
  fMsr2DataParam.global = false;
  fMsr2DataParam.globalPlus = false;

  // XML Parser part
  // 1st: check local directory
  QString path = QString("./");
  QString fln = QString("musredit_startup.xml");
  QString pathFln = path + fln;
  QProcessEnvironment procEnv = QProcessEnvironment::systemEnvironment();
  if (!QFile::exists(pathFln)) {
    // 2nd: check $HOME/.musrfit/musredit/musredit_startup.xml
    path = procEnv.value("HOME", "");
    pathFln = path + "/.musrfit/musredit/" + fln;
    if (!QFile::exists(pathFln)) {
      // 3rd: check $MUSRFITPATH/musredit_startup.xml
      path = procEnv.value("MUSRFITPATH", "");
      pathFln = path + "/" + fln;
      if (!QFile::exists(pathFln)) {
        // 4th: check $ROOTSYS/bin/musredit_startup.xml
        path = procEnv.value("ROOTSYS", "");
        pathFln = path + "/bin/" + fln;
        if (!QFile::exists(pathFln)) {
          // 5th: not found anyware hence create it
          path = procEnv.value("HOME", "");
          pathFln = path + "/.musrfit/musredit/" + fln;
          createMusreditStartupFile();
        }
      }
    }
  }
  fPrefPathName = pathFln;

  loadPrefs(fPrefPathName);
}

//--------------------------------------------------------------------------
/**
 * <p>Destructor
 */
PAdmin::~PAdmin()
{
  saveRecentFiles();
}

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
 * <p>returns the recent path-file name at position idx. If idx is out of scope,
 * an empty string is returned.
 *
 * \param idx index of the recent path-file name to be retrieved.
 */
QString PAdmin::getRecentFile(int idx)
{
  if (idx >= fRecentFile.size())
    return QString("");

  return fRecentFile[idx];
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
/**
 * <p>Loads the preference file fln.
 *
 * <b>return:</b> 1 on success, 0 otherwise
 *
 * \param fln path-file name of the preference file to be loaded.
 */
int PAdmin::loadPrefs(QString fln)
{
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
      return 0;
    }
  } else {
    QMessageBox::critical(0, "ERROR",
                          "Couldn't find the musredit_startup.xml settings file.\nProbably a few things will not work porperly.\nPlease fix this first.",
                          QMessageBox::Ok, QMessageBox::NoButton);
    return 0;
  }
  return 1;
}

//--------------------------------------------------------------------------
/**
 * <p>Save the preference file pref_fln.
 *
 * <b>return:</b> 1 on success, 0 otherwise
 *
 * \param pref_fln preference path-file name
 */
int PAdmin::savePrefs(QString pref_fln)
{
  // check if musredit_startup.xml is present in the current directory, and if yes, use this file to
  // save the recent file names otherwise use the "master" musredit_startup.xml
  QString fln = QString("./musredit_startup.xml");
  if (!QFile::exists(fln))
    fln = fPrefPathName;

  if (QFile::exists(fln)) { // administration file present
    QVector<QString> data;
    QFile file(fln);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
      cerr << endl << ">> PAdmin::savePrefs: **ERROR** Cannot open " << fln.toLatin1().data() << " for reading." << endl;
      return 1;
    }
    QTextStream fin(&file);
    while (!fin.atEnd()) {
      data.push_back(fin.readLine());
    }
    file.close();

    // replace all the prefs
    for (int i=0; i<data.size(); i++) {
      if (data[i].contains("<timeout>") && data[i].contains("</timeout>")) {
        data[i] = "    <timeout>" + QString("%1").arg(fTimeout) + "</timeout>";
      }
      if (data[i].contains("<keep_minuit2_output>") && data[i].contains("</keep_minuit2_output>")) {
        if (fKeepMinuit2Output)
          data[i] = "    <keep_minuit2_output>y</keep_minuit2_output>";
        else
          data[i] = "    <keep_minuit2_output>n</keep_minuit2_output>";
      }
      if (data[i].contains("<dump_ascii>") && data[i].contains("</dump_ascii>")) {
        if (fDumpAscii)
          data[i] = "    <dump_ascii>y</dump_ascii>";
        else
          data[i] = "    <dump_ascii>n</dump_ascii>";
      }
      if (data[i].contains("<dump_root>") && data[i].contains("</dump_root>")) {
        if (fDumpRoot)
          data[i] = "    <dump_root>y</dump_root>";
        else
          data[i] = "    <dump_root>n</dump_root>";
      }
      if (data[i].contains("<title_from_data_file>") && data[i].contains("</title_from_data_file>")) {
        if (fTitleFromDataFile)
          data[i] = "    <title_from_data_file>y</title_from_data_file>";
        else
          data[i] = "    <title_from_data_file>n</title_from_data_file>";
      }
      if (data[i].contains("<chisq_per_run_block>") && data[i].contains("</chisq_per_run_block>")) {
        if (fChisqPreRunBlock)
          data[i] = "    <chisq_per_run_block>y</chisq_per_run_block>";
        else
          data[i] = "    <chisq_per_run_block>n</chisq_per_run_block>";
      }
      if (data[i].contains("<estimate_n0>") && data[i].contains("</estimate_n0>")) {
        if (fEstimateN0)
          data[i] = "    <estimate_n0>y</estimate_n0>";
        else
          data[i] = "    <estimate_n0>n</estimate_n0>";
      }
      if (data[i].contains("<musrview_show_fourier>") && data[i].contains("</musrview_show_fourier>")) {
        if (fMusrviewShowFourier)
          data[i] = "    <musrview_show_fourier>y</musrview_show_fourier>";
        else
          data[i] = "    <musrview_show_fourier>n</musrview_show_fourier>";
      }
      if (data[i].contains("<musrview_show_avg>") && data[i].contains("</musrview_show_avg>")) {
        if (fMusrviewShowAvg)
          data[i] = "    <musrview_show_avg>y</musrview_show_avg>";
        else
          data[i] = "    <musrview_show_avg>n</musrview_show_avg>";
      }
      if (data[i].contains("<enable_musrt0>") && data[i].contains("</enable_musrt0>")) {
        if (fEnableMusrT0)
          data[i] = "    <enable_musrt0>y</enable_musrt0>";
        else
          data[i] = "    <enable_musrt0>n</enable_musrt0>";
      }
      if (data[i].contains("<font_name>") && data[i].contains("</font_name>")) {
        data[i] = QString("    <font_name>%1</font_name>").arg(fFontName);
      }
      if (data[i].contains("<font_size>") && data[i].contains("</font_size>")) {
        data[i] = QString("    <font_size>%1</font_size>").arg(fFontSize);
      }
    }

    // write prefs
    file.setFileName(pref_fln);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
      cerr << endl << ">> PAdmin::savePrefs: **ERROR** Cannot open " << pref_fln.toLatin1().data() << " for writing." << endl;
      return 0;
    }
    fin.setDevice(&file);
    for (int i=0; i<data.size(); i++)
      fin << data[i] << endl;
    file.close();
  } else {
    QString msg("Failed to write musredit_startup.xml. Neither a local nor a global copy found.");
    QMessageBox::warning(0, "WARNING", msg, QMessageBox::Ok, QMessageBox::NoButton);
  }

  return 1;
}

//--------------------------------------------------------------------------
/**
 * <p>Add recent path-file name to the internal ring-buffer.
 *
 * \param str recent path-file name to be added
 */
void PAdmin::addRecentFile(const QString str)
{
  // check if file name is not already present
  for (int i=0; i<fRecentFile.size(); i++) {
    if (str == fRecentFile[i])
      return;
  }

  fRecentFile.push_front(str);
  if (fRecentFile.size() > MAX_RECENT_FILES)
    fRecentFile.resize(MAX_RECENT_FILES);
}

//--------------------------------------------------------------------------
/**
 * <p>Merges the recent file ring buffer into musredit_startup.xml and saves it.
 * If a local copy is present it will be saved there, otherwise the master file
 * will be used.
 */
void PAdmin::saveRecentFiles()
{
  // check if musredit_startup.xml is present in the current directory, and if yes, use this file to
  // save the recent file names otherwise use the "master" musredit_startup.xml

  QString str("");
  QString fln = QString("./musredit_startup.xml");
  if (!QFile::exists(fln))
    fln = fPrefPathName;

  if (QFile::exists(fln)) { // administration file present
    QVector<QString> data;
    QFile file(fln);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
      cerr << endl << ">> PAdmin::saveRecentFile: **ERROR** Cannot open " << fln.toLatin1().data() << " for reading." << endl;
      return;
    }
    QTextStream fin(&file);
    while (!fin.atEnd()) {
      data.push_back(fin.readLine());
    }
    file.close();

    // remove <path_file_name> from data
    for (QVector<QString>::iterator it = data.begin(); it != data.end(); ++it) {
      if (it->contains("<path_file_name>")) {
        it = data.erase(it);
        --it;
      }
    }

    // add recent files
    int i;
    for (i=0; i<data.size(); i++) {
      if (data[i].contains("<recent_files>"))
        break;
    }

    if (i == data.size()) {
      cerr << endl << ">> PAdmin::saveRecentFile: **ERROR** " << fln.toLatin1().data() << " seems to be corrupt." << endl;
      return;
    }
    i++;
    for (int j=0; j<fRecentFile.size(); j++) {
      str = "    <path_file_name>" + fRecentFile[j] + "</path_file_name>";
      data.insert(i++, str);
    }

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
      cerr << endl << ">> PAdmin::saveRecentFile: **ERROR** Cannot open " << fln.toLatin1().data() << " for reading." << endl;
      return;
    }
    fin.setDevice(&file);
    for (int i=0; i<data.size(); i++)
      fin << data[i] << endl;
    file.close();
  } else {
    QString msg("Failed to write musredit_startup.xml. Neither a local nor a global copy found.");
    QMessageBox::warning(0, "WARNING", msg, QMessageBox::Ok, QMessageBox::NoButton);
  }
}

//--------------------------------------------------------------------------
/**
 * @brief PAdmin::createMusreditStartupFile
 */
void PAdmin::createMusreditStartupFile()
{
  // get $HOME
  QProcessEnvironment procEnv = QProcessEnvironment::systemEnvironment();
  QString pathName = procEnv.value("HOME", "");
  pathName += "/.musrfit/musredit";

  // check if the directory $HOME/.musrfit/musredit exists if not create it
  QDir dir(pathName);
  if (!dir.exists()) {
    // directory $HOME/.musrfit/musredit does not exist hence create it
    dir.mkpath(pathName);
  }

  // create default musredit_startup.xml file in $HOME/.musrfit/musredit
  pathName += "/musredit_startup.xml";
  QFile file(pathName);

  if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    return;

  QTextStream fout(&file);

  fout << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
  fout << "<musredit_startup xmlns=\"http://lmu.web.psi.ch/musrfit/user/MUSR/MusrGui.html\">" << endl;
  fout << "  <comment>" << endl;
  fout << "    This is handling default setting parameters for the musredit." << endl;
  fout << "  </comment>" << endl;
  fout << "  <general>" << endl;
  fout << "    <exec_path>" << MUSRFIT_PREFIX << "/bin</exec_path>" << endl;
  fout << "    <default_save_path>./</default_save_path>" << endl;
  fout << "    <msr_default_file_path>" << MUSRFIT_DOC_DIR << "/templates</msr_default_file_path>" << endl;
  fout << "    <timeout>3600</timeout>" << endl;
  fout << "    <keep_minuit2_output>n</keep_minuit2_output>" << endl;
  fout << "    <dump_ascii>n</dump_ascii>" << endl;
  fout << "    <dump_root>n</dump_root>" << endl;
  fout << "    <title_from_data_file>y</title_from_data_file>" << endl;
  fout << "    <chisq_per_run_block>n</chisq_per_run_block>" << endl;
  fout << "    <estimate_n0>y</estimate_n0>" << endl;
  fout << "    <musrview_show_fourier>n</musrview_show_fourier>" << endl;
  fout << "    <musrview_show_avg>n</musrview_show_avg>" << endl;
  fout << "    <enable_musrt0>y</enable_musrt0>" << endl;
  fout << "  </general>" << endl;
  fout << "  <recent_files>" << endl;
  fout << "    <path_file_name>" << MUSRFIT_DOC_DIR << "/examples/test-histo-PSI-BIN.msr</path_file_name>" << endl;
  fout << "  </recent_files>" << endl;
  fout << "  <help_section>" << endl;
  fout << "    <musr_web_main>file://" << MUSRFIT_DOC_DIR << "/html/user/MUSR/MusrFit.html</musr_web_main>" << endl;
  fout << "    <musr_web_title>file://" << MUSRFIT_DOC_DIR << "/html/user/MUSR/MusrFit.html#TheTitle</musr_web_title>" << endl;
  fout << "    <musr_web_parameters>file://" << MUSRFIT_DOC_DIR << "/html/user/MUSR/MusrFit.html#TheFitparameterBlock</musr_web_parameters>" << endl;
  fout << "    <musr_web_theory>file://" << MUSRFIT_DOC_DIR << "/html/user/MUSR/MusrFit.html#TheTheoryBlock</musr_web_theory>" << endl;
  fout << "    <musr_web_functions>file://" << MUSRFIT_DOC_DIR << "/html/user/MUSR/MusrFit.html#TheFunctionsBlock</musr_web_functions>" << endl;
  fout << "    <musr_web_run>file://" << MUSRFIT_DOC_DIR << "/html/user/MUSR/MusrFit.html#TheRunBlock</musr_web_run>" << endl;
  fout << "    <musr_web_command>file://" << MUSRFIT_DOC_DIR << "/html/user/MUSR/MusrFit.html#TheCommandsBlock</musr_web_command>" << endl;
  fout << "    <musr_web_fourier>file://" << MUSRFIT_DOC_DIR << "/html/user/MUSR/MusrFit.html#TheFourierBlock</musr_web_fourier>" << endl;
  fout << "    <musr_web_plot>file://" << MUSRFIT_DOC_DIR << "/html/user/MUSR/MusrFit.html#ThePlotBlock</musr_web_plot>" << endl;
  fout << "    <musr_web_statistic>file://" << MUSRFIT_DOC_DIR << "/html/user/MUSR/MusrFit.html#TheStatisticBlock</musr_web_statistic>" << endl;
  fout << "    <musr_web_msr2data>file://" << MUSRFIT_DOC_DIR << "/html/user/MUSR/Msr2Data.html</musr_web_msr2data>" << endl;
  fout << "    <musr_web_musrFT>file://" << MUSRFIT_DOC_DIR << "/html/user/MUSR/MusrFit.html#A_2.3_musrFT</musr_web_musrFT>" << endl;
  fout << "  </help_section>" << endl;
  fout << "  <font_settings>" << endl;
#ifdef Q_OS_MAC
  fout << "    <font_name>Courier New</font_name>" << endl;
  fout << "    <font_size>16</font_size>" << endl;
#else
  fout << "    <font_name>Monospace</font_name>" << endl;
  fout << "    <font_size>12</font_size>" << endl;
#endif
  fout << "  </font_settings>" << endl;
  fout << "  <msr_file_defaults>" << endl;
  fout << "    <beamline>mue4</beamline>" << endl;
  fout << "    <institute>psi</institute>" << endl;
  fout << "    <file_format>root-npp</file_format>" << endl;
  fout << "    <lifetime_correction>y</lifetime_correction>" << endl;
  fout << "  </msr_file_defaults>" << endl;
  fout << "  <msr2data_defaults>" << endl;
  fout << "    <chain_fit>y</chain_fit>" << endl;
  fout << "    <write_data_header>y</write_data_header>" << endl;
  fout << "    <ignore_data_header_info>n</ignore_data_header_info>" << endl;
  fout << "    <keep_minuit2_output>n</keep_minuit2_output>" << endl;
  fout << "    <write_column_data>n</write_column_data>" << endl;
  fout << "    <recreate_data_file>n</recreate_data_file>" << endl;
  fout << "    <open_file_after_fitting>y</open_file_after_fitting>" << endl;
  fout << "    <create_msr_file_only>n</create_msr_file_only>" << endl;
  fout << "    <fit_only>n</fit_only>" << endl;
  fout << "    <global>n</global>" << endl;
  fout << "    <global_plus>n</global_plus>" << endl;
  fout << "  </msr2data_defaults>" << endl;
  fout << "  <func_pixmap_path>" << MUSRFIT_DOC_DIR << "/latex_images</func_pixmap_path>" << endl;
  fout << "  <theory_functions>" << endl;
  fout << "    <func>" << endl;
  fout << "      <name>asymmetry</name>" << endl;
  fout << "      <comment></comment>" << endl;
  fout << "      <label>Asymmetry</label>" << endl;
  fout << "      <pixmap>asymmetry.png</pixmap>" << endl;
  fout << "      <params>1</params>" << endl;
  fout << "    </func>" << endl;
  fout << "    <func>" << endl;
  fout << "      <name>simplExpo</name>" << endl;
  fout << "      <comment>(rate)</comment>" << endl;
  fout << "      <label>simple Exp</label>" << endl;
  fout << "      <pixmap>simpleExp.png</pixmap>" << endl;
  fout << "      <params>1</params>" << endl;
  fout << "    </func>" << endl;
  fout << "    <func>" << endl;
  fout << "      <name>generExpo</name>" << endl;
  fout << "      <comment>(rate exponent)</comment>" << endl;
  fout << "      <label>general Exp</label>" << endl;
  fout << "      <pixmap>generalExp.png</pixmap>" << endl;
  fout << "      <params>2</params>" << endl;
  fout << "    </func>" << endl;
  fout << "    <func>" << endl;
  fout << "      <name>simpleGss</name>" << endl;
  fout << "      <comment>(rate)</comment>" << endl;
  fout << "      <label>simple Gauss</label>" << endl;
  fout << "      <pixmap>simpleGauss.png</pixmap>" << endl;
  fout << "      <params>1</params>" << endl;
  fout << "    </func>" << endl;
  fout << "    <func>" << endl;
  fout << "      <name>statGssKT</name>" << endl;
  fout << "      <comment>(rate)</comment>" << endl;
  fout << "      <label>static Gauss KT</label>" << endl;
  fout << "      <pixmap>statGssKT.png</pixmap>" << endl;
  fout << "      <params>1</params>" << endl;
  fout << "    </func>" << endl;
  fout << "    <func>" << endl;
  fout << "      <name>statGssKTLF</name>" << endl;
  fout << "      <comment>(frequency damping)</comment>" << endl;
  fout << "      <label>static Gauss KT LF</label>" << endl;
  fout << "      <pixmap></pixmap>" << endl;
  fout << "      <params>2</params>" << endl;
  fout << "    </func>" << endl;
  fout << "    <func>" << endl;
  fout << "      <name>dynGssKTLF</name>" << endl;
  fout << "      <comment>(frequency damping hopping-rate)</comment>" << endl;
  fout << "      <label>dynamic Gauss KT LF</label>" << endl;
  fout << "      <pixmap></pixmap>" << endl;
  fout << "      <params>3</params>" << endl;
  fout << "    </func>" << endl;
  fout << "    <func>" << endl;
  fout << "      <name>statExpKT</name>" << endl;
  fout << "      <comment>(rate)</comment>" << endl;
  fout << "      <label>static Lorentz KT</label>" << endl;
  fout << "      <pixmap>statExpKT.png</pixmap>" << endl;
  fout << "      <params>1</params>" << endl;
  fout << "    </func>" << endl;
  fout << "    <func>" << endl;
  fout << "      <name>statExpKTLF</name>" << endl;
  fout << "      <comment>(frequency damping)</comment>" << endl;
  fout << "      <label>static Lorentz KT LF</label>" << endl;
  fout << "      <pixmap></pixmap>" << endl;
  fout << "      <params>2</params>" << endl;
  fout << "    </func>" << endl;
  fout << "    <func>" << endl;
  fout << "      <name>dynExpKTLF</name>" << endl;
  fout << "      <comment>(frequency damping hopping-rate)</comment>" << endl;
  fout << "      <label>dynamic Lorentz KT LF</label>" << endl;
  fout << "      <pixmap></pixmap>" << endl;
  fout << "      <params>3</params>" << endl;
  fout << "    </func>" << endl;
  fout << "    <func>" << endl;
  fout << "      <name>combiLGKT</name>" << endl;
  fout << "      <comment>(lorentzRate gaussRate)</comment>" << endl;
  fout << "      <label>combined Lorentz-Gauss KT</label>" << endl;
  fout << "      <pixmap>combiLGKT.png</pixmap>" << endl;
  fout << "      <params>2</params>" << endl;
  fout << "    </func>" << endl;
  fout << "    <func>" << endl;
  fout << "      <name>strKT</name>" << endl;
  fout << "      <comment>(rate beta)</comment>" << endl;
  fout << "      <label>stretched Kubo-Toyabe</label>" << endl;
  fout << "      <pixmap></pixmap>" << endl;
  fout << "      <params>2</params>" << endl;
  fout << "    </func>" << endl;
  fout << "    <func>" << endl;
  fout << "      <name>spinGlass</name>" << endl;
  fout << "      <comment>(rate hopping-rate order)</comment>" << endl;
  fout << "      <label>zero field spin glass function</label>" << endl;
  fout << "      <pixmap></pixmap>" << endl;
  fout << "      <params>3</params>" << endl;
  fout << "    </func>" << endl;
  fout << "    <func>" << endl;
  fout << "      <name>rdAnisoHf</name>" << endl;
  fout << "      <comment>(frequency rate)</comment>" << endl;
  fout << "      <label>random anisotropic hyperfine function</label>" << endl;
  fout << "      <pixmap></pixmap>" << endl;
  fout << "      <params>2</params>" << endl;
  fout << "    </func>" << endl;
  fout << "    <func>" << endl;
  fout << "      <name>abragam</name>" << endl;
  fout << "      <comment>(rate hopping-rate)</comment>" << endl;
  fout << "      <label>Abragam</label>" << endl;
  fout << "      <pixmap>abragam.png</pixmap>" << endl;
  fout << "      <params>2</params>" << endl;
  fout << "    </func>" << endl;
  fout << "    <func>" << endl;
  fout << "      <name>TFieldCos</name>" << endl;
  fout << "      <comment>(phase frequency)</comment>" << endl;
  fout << "      <label>TF cos</label>" << endl;
  fout << "      <pixmap>tfCos.png</pixmap>" << endl;
  fout << "      <params>2</params>" << endl;
  fout << "    </func>" << endl;
  fout << "    <func>" << endl;
  fout << "      <name>internFld</name>" << endl;
  fout << "      <comment>(fraction phase frequency Trate Lrate)</comment>" << endl;
  fout << "      <label>internal Lorentz field</label>" << endl;
  fout << "      <pixmap>internalField.png</pixmap>" << endl;
  fout << "      <params>5</params>" << endl;
  fout << "    </func>" << endl;
  fout << "    <func>" << endl;
  fout << "      <name>internFldGK</name>" << endl;
  fout << "      <comment>(fraction frequency Trate Lrate beta)</comment>" << endl;
  fout << "      <label>internal field, Gaussian broadened</label>" << endl;
  fout << "      <pixmap>internalFieldGK.png</pixmap>" << endl;
  fout << "      <params>5</params>" << endl;
  fout << "    </func>" << endl;
  fout << "    <func>" << endl;
  fout << "      <name>internFldLL</name>" << endl;
  fout << "      <comment>(fraction frequency Trate Lrate)</comment>" << endl;
  fout << "      <label>internal field, Lorentzian broadened</label>" << endl;
  fout << "      <pixmap>internalFieldLL.png</pixmap>" << endl;
  fout << "      <params>4</params>" << endl;
  fout << "    </func>" << endl;
  fout << "    <func>" << endl;
  fout << "      <name>bessel</name>" << endl;
  fout << "      <comment>(phase frequency)</comment>" << endl;
  fout << "      <label>spherical Bessel 0th order</label>" << endl;
  fout << "      <pixmap>bessel.png</pixmap>" << endl;
  fout << "      <params>2</params>" << endl;
  fout << "    </func>" << endl;
  fout << "    <func>" << endl;
  fout << "      <name>internBsl</name>" << endl;
  fout << "      <comment>(fraction phase frequency Trate Lrate)</comment>" << endl;
  fout << "      <label>static internal Bessel</label>" << endl;
  fout << "      <pixmap>internalBessel.png</pixmap>" << endl;
  fout << "      <params>5</params>" << endl;
  fout << "    </func>" << endl;
  fout << "    <func>" << endl;
  fout << "      <name>skewedGss</name>" << endl;
  fout << "      <comment>(phase frequency rate_m rate_p)</comment>" << endl;
  fout << "      <label>skewed Gaussian</label>" << endl;
  fout << "      <pixmap></pixmap>" << endl;
  fout << "      <params>4</params>" << endl;
  fout << "    </func>" << endl;
  fout << "    <func>" << endl;
  fout << "      <name>staticNKZF</name>" << endl;
  fout << "      <comment>(damping_D0 R_b)</comment>" << endl;
  fout << "      <label>static Noakes-Kalvius ZF</label>" << endl;
  fout << "      <pixmap></pixmap>" << endl;
  fout << "      <params>2</params>" << endl;
  fout << "    </func>" << endl;
  fout << "    <func>" << endl;
  fout << "      <name>staticNKTF</name>" << endl;
  fout << "      <comment>(phase frequency damping_D0 R_b)</comment>" << endl;
  fout << "      <label>static Noakes-Kalvius TF</label>" << endl;
  fout << "      <pixmap></pixmap>" << endl;
  fout << "      <params>4</params>" << endl;
  fout << "    </func>" << endl;
  fout << "    <func>" << endl;
  fout << "      <name>dynamicNKZF</name>" << endl;
  fout << "      <comment>(damping_D0 R_b nu_c)</comment>" << endl;
  fout << "      <label>dynamic Noakes-Kalvius ZF</label>" << endl;
  fout << "      <pixmap></pixmap>" << endl;
  fout << "      <params>3</params>" << endl;
  fout << "    </func>" << endl;
  fout << "    <func>" << endl;
  fout << "      <name>dynamicNKTF</name>" << endl;
  fout << "      <comment>(phase frequency damping_D0 R_b nu_c)</comment>" << endl;
  fout << "      <label>dynamic Noakes-Kalvius TF</label>" << endl;
  fout << "      <pixmap></pixmap>" << endl;
  fout << "      <params>5</params>" << endl;
  fout << "    </func>" << endl;
  fout << "    <func>" << endl;
  fout << "      <name>polynom</name>" << endl;
  fout << "      <comment>(tshift p0 p1 ... pn)</comment>" << endl;
  fout << "      <label>polynom</label>" << endl;
  fout << "      <pixmap>polynom.png</pixmap>" << endl;
  fout << "      <params>4</params>" << endl;
  fout << "    </func>" << endl;
  fout << "    <func>" << endl;
  fout << "      <name>userFcn</name>" << endl;
  fout << "      <comment></comment>" << endl;
  fout << "      <label>user function</label>" << endl;
  fout << "      <pixmap></pixmap>" << endl;
  fout << "      <params>0</params>" << endl;
  fout << "    </func>" << endl;
  fout << "  </theory_functions>" << endl;
  fout << "</musredit_startup>" << endl;

  file.close();
}

//--------------------------------------------------------------------------
// END
//--------------------------------------------------------------------------
