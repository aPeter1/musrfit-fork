/****************************************************************************

  PAdmin.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

*****************************************************************************/

/***************************************************************************
 *   Copyright (C) 2010-2021 by Andreas Suter                              *
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

#include <QMessageBox>
#include <QString>
#include <QStringView>
#include <QLatin1String>
#include <QFile>
#include <QTextStream>
#include <QVector>
#include <QDir>
#include <QProcessEnvironment>
#include <QSysInfo>
#include <QIODevice>

#include "musrfit-info.h"
#include "PAdmin.h"

//--------------------------------------------------------------------------
// implementation of PAdminXMLParser class
//--------------------------------------------------------------------------
/**
 * <p>XML Parser class for the musredit administration file.
 *
 * \param fln file name of the musredit startup xml-file
 * \param admin pointer to an admin class instance.
 */
PAdminXMLParser::PAdminXMLParser(const QString& fln, PAdmin *admin) : fAdmin(admin)
{
  fValid = false;
  fKeyWord = eEmpty;
  fFunc = false;

  QFile file(fln);
  if (!file.open(QFile::ReadOnly | QFile::Text)) {
    // warning and create default - STILL MISSING
  }

  fValid = parse(&file);
}

//--------------------------------------------------------------------------
/**
 * <p>parse the musredit startup xml-file.
 *
 * \param device QFile object of the musredit startup xml-file
 *
 * @return true on success, false otherwise
 */
bool PAdminXMLParser::parse(QIODevice *device)
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
bool PAdminXMLParser::startDocument()
{
  // nothing to be done here for now
  return true;
}

//--------------------------------------------------------------------------
/**
 * <p>Routine called when a new XML tag is found. Here it is used
 * to set a tag for filtering afterwards the content.
 */
bool PAdminXMLParser::startElement()
{
  QString qName = fXml.name().toString();

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
  } else if (qName == "musrview_show_one_to_one") {
    fKeyWord = eMusrviewShowOneToOne;
  } else if (qName == "enable_musrt0") {
    fKeyWord = eEnableMusrT0;
  } else if (qName == "dark_theme_icons_menu") {
    fKeyWord = eDarkThemeIconsMenu;
  } else if (qName == "dark_theme_icons_toolbar") {
    fKeyWord = eDarkThemeIconsToolbar;
  } else if (qName == "edit_w") {
    fKeyWord = eEditW;
  } else if (qName == "edit_h") {
    fKeyWord = eEditH;
  } else if (qName =="keep_minuit2_output") {
    fKeyWord = eKeepMinuit2Output;
  } else if (qName == "dump_ascii") {
    fKeyWord = eDumpAscii;
  } else if (qName =="dump_root") {
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
    fTheoryItem.pixmap = QPixmap();
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
 */
bool PAdminXMLParser::endElement()
{
  fKeyWord = eEmpty;

  QString qName = fXml.name().toString();

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
bool PAdminXMLParser::characters()
{
  QString str = fXml.text().toString();

  if (str.isEmpty())
    return true;

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
    case eMusrviewShowOneToOne:
      if (str == "y")
        flag = true;
      else
        flag = false;
      fAdmin->setMusrviewShowOneToOneFlag(flag);
      break;
    case eEnableMusrT0:
      if (str == "y")
        flag = true;
      else
        flag = false;
      fAdmin->setEnableMusrT0Flag(flag);
      break;
    case eDarkThemeIconsMenu:
      if (str == "y")
        flag = true;
      else
        flag = false;
      fAdmin->setDarkThemeIconsMenuFlag(flag);
      break;
    case eDarkThemeIconsToolbar:
      if (str == "y")
        flag = true;
      else
        flag = false;
      fAdmin->setDarkThemeIconsToolbarFlag(flag);
      break;
    case eEditW:
      ival = QString(str.toLatin1()).trimmed().toInt(&ok);
      if (ok)
        fAdmin->setEditWidth(ival);
      break;
    case eEditH:
      ival = QString(str.toLatin1()).trimmed().toInt(&ok);
      if (ok)
        fAdmin->setEditHeight(ival);
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

  if (fAdmin->getTheoFuncPixmapPath().indexOf('$') >=0) {
    str = expandPath(fAdmin->getTheoFuncPixmapPath());
    if (!str.isEmpty())
      fAdmin->setTheoFuncPixmapPath(str);
  }

  //as35 dump admin - only for debugging
  //as35 dump();

  return true;
}

//--------------------------------------------------------------------------
/**
 * <p>Dumps the content of the admin class. For debugging purposes only.
 */
void PAdminXMLParser::dump()
{
  std::cout << "debug> +++++++++++++++++++++++" << std::endl;
  std::cout << "debug> general:" << std::endl;
  std::cout << "debug> exec_path : " << fAdmin->getExecPath().toLatin1().data() << std::endl;
  std::cout << "debug> default_save_path : " << fAdmin->getDefaultSavePath().toLatin1().data() << std::endl;
  std::cout << "debug> timeout : " << fAdmin->getTimeout() << std::endl;
  std::cout << "debug> keep_minuit2_output : " << fAdmin->getKeepMinuit2OutputFlag() << std::endl;
  std::cout << "debug> dump_ascii : " << fAdmin->getDumpAsciiFlag() << std::endl;
  std::cout << "debug> dump_root : " << fAdmin->getDumpRootFlag() << std::endl;
  std::cout << "debug> title_from_data_file : " << fAdmin->getTitleFromDataFileFlag() << std::endl;
  std::cout << "debug> chisq_per_run_block : " << fAdmin->getChisqPerRunBlockFlag() << std::endl;
  std::cout << "debug> estimate_n0 : " << fAdmin->getEstimateN0Flag() << std::endl;
  std::cout << "debug> musrview_show_fourier : " << fAdmin->getMusrviewShowFourierFlag() << std::endl;
  std::cout << "debug> musrview_show_avg : " << fAdmin->getMusrviewShowAvgFlag() << std::endl;
  std::cout << "debug> enable_musrt0 : " << fAdmin->getEnableMusrT0Flag() << std::endl;
  std::cout << "debug> dark_theme_icons_menu : " << fAdmin->getDarkThemeIconsMenuFlag() << std::endl;
  std::cout << "debug> dark_theme_icons_toolbar : " << fAdmin->getDarkThemeIconsToolbarFlag() << std::endl;
  std::cout << "debug> edit_w : " << fAdmin->getEditWidth() << std::endl;
  std::cout << "debug> edit_h : " << fAdmin->getEditHeight() << std::endl;
  std::cout << "debug> +++++++++++++++++++++++" << std::endl;
  std::cout << "debug> recent_files:" << std::endl;
  for (int i=0; i<fAdmin->getNumRecentFiles(); i++) {
    std::cout << "debug> recent_file " << i << ":" << fAdmin->getRecentFile(i).toLatin1().data() << std::endl;
  }
  std::cout << "debug> +++++++++++++++++++++++" << std::endl;
  std::cout << "debug> help_section:" << std::endl;
  std::cout << "debug> musr_web_main : " << fAdmin->getHelpUrl("main").toLatin1().data() << std::endl;
  std::cout << "debug> musr_web_title : " << fAdmin->getHelpUrl("title").toLatin1().data() << std::endl;
  std::cout << "debug> musr_web_parameters : " << fAdmin->getHelpUrl("parameters").toLatin1().data() << std::endl;
  std::cout << "debug> musr_web_theory : " << fAdmin->getHelpUrl("theory").toLatin1().data() << std::endl;
  std::cout << "debug> musr_web_functions : " << fAdmin->getHelpUrl("functions").toLatin1().data() << std::endl;
  std::cout << "debug> musr_web_run : " << fAdmin->getHelpUrl("run").toLatin1().data() << std::endl;
  std::cout << "debug> musr_web_command : " << fAdmin->getHelpUrl("command").toLatin1().data() << std::endl;
  std::cout << "debug> musr_web_fourier : " << fAdmin->getHelpUrl("fourier").toLatin1().data() << std::endl;
  std::cout << "debug> musr_web_plot : " << fAdmin->getHelpUrl("plot").toLatin1().data() << std::endl;
  std::cout << "debug> musr_web_statistic : " << fAdmin->getHelpUrl("statistic").toLatin1().data() << std::endl;
  std::cout << "debug> musr_web_msr2data : " << fAdmin->getHelpUrl("msr2data").toLatin1().data() << std::endl;
  std::cout << "debug> musr_web_musrFT : " << fAdmin->getHelpUrl("musrFT").toLatin1().data() << std::endl;
  std::cout << "debug> +++++++++++++++++++++++" << std::endl;
  std::cout << "debug> font_section:" << std::endl;
  std::cout << "debug> font_name : " << fAdmin->getFontName().toLatin1().data() << std::endl;
  std::cout << "debug> font_size : " << fAdmin->getFontSize() << std::endl;
  std::cout << "debug> +++++++++++++++++++++++" << std::endl;
  std::cout << "debug> msr_file_defaults:" << std::endl;
  std::cout << "debug> beamline : " << fAdmin->getBeamline().toLatin1().data() << std::endl;
  std::cout << "debug> institute : " << fAdmin->getInstitute().toLatin1().data() << std::endl;
  std::cout << "debug> file_format : " << fAdmin->getFileFormat().toLatin1().data() << std::endl;
  std::cout << "debug> lifetime_correction : " << fAdmin->getLifetimeCorrectionFlag() << std::endl;
  std::cout << "debug> +++++++++++++++++++++++" << std::endl;
  std::cout << "debug> msr2data_defaults:" << std::endl;
  std::cout << "debug> chain_fit : " << fAdmin->getMsr2DataParam().chainFit << std::endl;
  std::cout << "debug> write_data_header : " << fAdmin->getMsr2DataParam().writeDbHeader << std::endl;
  std::cout << "debug> ignore_data_header_info : " << fAdmin->getMsr2DataParam().ignoreDataHeaderInfo << std::endl;
  std::cout << "debug> keep_minuit2_output : " << fAdmin->getMsr2DataParam().keepMinuit2Output << std::endl;
  std::cout << "debug> write_column_data : " << fAdmin->getMsr2DataParam().writeColumnData << std::endl;
  std::cout << "debug> recreate_data_file : " << fAdmin->getMsr2DataParam().recreateDbFile << std::endl;
  std::cout << "debug> open_file_after_fitting : " << fAdmin->getMsr2DataParam().openFilesAfterFitting << std::endl;
  std::cout << "debug> create_msr_file_only : " << fAdmin->getMsr2DataParam().createMsrFileOnly << std::endl;
  std::cout << "debug> fit_only : " << fAdmin->getMsr2DataParam().fitOnly << std::endl;
  std::cout << "debug> global : " << fAdmin->getMsr2DataParam().global << std::endl;
  std::cout << "debug> global_plus : " << fAdmin->getMsr2DataParam().globalPlus << std::endl;
  std::cout << "debug> +++++++++++++++++++++++" << std::endl;
  std::cout << "debug> theory_functions:" << std::endl;
  std::cout << "debug> func_pixmap_path : " << fAdmin->getTheoFuncPixmapPath().toLatin1().data() << std::endl;
  std::cout << "debug> #theory : " << fAdmin->getTheoryCounts() << std::endl;
  PTheory *theo;
  for (unsigned int i=0; i<fAdmin->getTheoryCounts(); i++) {
    theo = fAdmin->getTheoryItem(i);
    std::cout << "debug> -------" << std::endl;
    std::cout << "debug> name : " << theo->name.toLatin1().data() << std::endl;
    std::cout << "debug> comment : " << theo->comment.toLatin1().data() << std::endl;
    std::cout << "debug> label : " << theo->label.toLatin1().data() << std::endl;
    std::cout << "debug> pixmapName : " << theo->pixmapName.toLatin1().data() << std::endl;
    std::cout << "debug> params : " << theo->params << std::endl;
  }
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
        QMessageBox::warning(nullptr, "WARNING", msg, QMessageBox::Ok, QMessageBox::NoButton);
        newStr = "";
        break;
      }
      path = procEnv.value(token, "");
      if (path.isEmpty()) {
        msg = QString("Couldn't expand '%1'. Some things might not work properly").arg(token);
        QMessageBox::warning(nullptr, "WARNING", msg, QMessageBox::Ok, QMessageBox::NoButton);
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
          // 5th: not found anywhere hence create it
          path = procEnv.value("HOME", "");
          pathFln = path + "/.musrfit/musredit/" + fln;
          createMusreditStartupFile();
        }
      }
    }
  }
  fPrefPathName = pathFln;

  loadPrefs(fPrefPathName);

  // make sure that musrfit, musrview, etc are found under the fExecPath provided
  QString str = fExecPath + "/musrfit";
  QFileInfo info(str);
  if (info.exists()) {
    if (!info.isExecutable())
      QMessageBox::critical(nullptr, "ERROR", "musrfit found but not recognized as executable.\nPlease check!");
  } else {
    QMessageBox::critical(nullptr, "ERROR", "musrfit not found.\nHave you set the necessary system variables properly?\nPlease check the manual.\nBefore you can use musrfit, this needs to be fixed.");
  }

  // check if system variables are set properly
  bool sysVarMissing = false;
  QString msg("Missing System Variables:\n");
  path = procEnv.value("ROOTSYS", "");
  if (path.isEmpty()) {
    msg += "> ROOTSYS\n";
    sysVarMissing = true;
  }
  path = procEnv.value("MUSRFITPATH", "");
  if (path.isEmpty()) {
    msg += "> MUSRFITPATH\n";
    sysVarMissing = true;
  }
  if (sysVarMissing) {
    msg += "Please set this/these system variables.";
    QMessageBox::warning(nullptr, "WARNING", msg);
  }
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
    PAdminXMLParser handler(fln, this);
    if (!handler.isValid()) {
      QMessageBox::critical(nullptr, "ERROR",
                            "Error parsing musredit_startup.xml settings file.\nProbably a few things will not work porperly.\nPlease fix this first.",
                            QMessageBox::Ok, QMessageBox::NoButton);
      return 0;
    }
  } else {
    QMessageBox::critical(nullptr, "ERROR",
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
      std::cerr << std::endl << ">> PAdmin::savePrefs: **ERROR** Cannot open " << fln.toLatin1().data() << " for reading." << std::endl;
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
      if (data[i].contains("<dark_theme_icons_menu>") && data[i].contains("</dark_theme_icons_menu>")) {
        if (fDarkThemeIconsMenu)
          data[i] = "     <dark_theme_icons_menu>y</dark_theme_icons_menu>";
        else
          data[i] = "     <dark_theme_icons_menu>n</dark_theme_icons_menu>";
      }
      if (data[i].contains("<dark_theme_icons_toolbar>") && data[i].contains("</dark_theme_icons_toolbar>")) {
        if (fDarkThemeIconsToolbar)
          data[i] = "     <dark_theme_icons_toolbar>y</dark_theme_icons_toolbar>";
        else
          data[i] = "     <dark_theme_icons_toolbar>n</dark_theme_icons_toolbar>";
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
      std::cerr << std::endl << ">> PAdmin::savePrefs: **ERROR** Cannot open " << pref_fln.toLatin1().data() << " for writing." << std::endl;
      return 0;
    }
    fin.setDevice(&file);
    for (int i=0; i<data.size(); i++) {
      fin << data[i] << Qt::endl;
    }
    file.close();
  } else {
    QString msg("Failed to write musredit_startup.xml. Neither a local nor a global copy found.");
    QMessageBox::warning(nullptr, "WARNING", msg, QMessageBox::Ok, QMessageBox::NoButton);
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
      std::cerr << std::endl << ">> PAdmin::saveRecentFile: **ERROR** Cannot open " << fln.toLatin1().data() << " for reading." << std::endl;
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
      std::cerr << std::endl << ">> PAdmin::saveRecentFile: **ERROR** " << fln.toLatin1().data() << " seems to be corrupt." << std::endl;
      return;
    }
    i++;
    for (int j=0; j<fRecentFile.size(); j++) {
      str = "    <path_file_name>" + fRecentFile[j] + "</path_file_name>";
      data.insert(i++, str);
    }

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
      std::cerr << std::endl << ">> PAdmin::saveRecentFile: **ERROR** Cannot open " << fln.toLatin1().data() << " for reading." << std::endl;
      return;
    }
    fin.setDevice(&file);
    for (int i=0; i<data.size(); i++) {
      fin << data[i] << Qt::endl;
    }
    file.close();
  } else {
    QString msg("Failed to write musredit_startup.xml. Neither a local nor a global copy found.");
    QMessageBox::warning(nullptr, "WARNING", msg, QMessageBox::Ok, QMessageBox::NoButton);
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

  // get the default musredit_startup.xml.in from the internal resources
  QFile fres(":/musredit_startup.xml.in");
  if (!fres.exists()) {
    QString msg = QString("Neither couldn't find nor create musredit_startup.xml. Things are likely not to work.");
    QMessageBox::critical(nullptr, "ERROR", msg);
    return;
  }

  if (!fres.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QString msg = QString("Couldn't open internal resource file musredit_startup.xml.in. Things are likely not to work.");
    QMessageBox::critical(nullptr, "ERROR", msg);
    return;
  }
  // text stream for fres
  QTextStream fin(&fres);

  // musredit_startup.xml default file
  QFile file(pathName);

  if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    return;

  // text stream for file
  QTextStream fout(&file);

  QString line;
  while (!fin.atEnd()) {
    line = fin.readLine();
    line.replace("@prefix@", MUSRFIT_PREFIX);
    line.replace("@DOCDIR@", MUSRFIT_DOC_DIR);
    if (line.contains("<font_name>")) { // defaults: linux: Monospace, macOS: Courier New
      if (QSysInfo::productType().contains("osx")) {
        line.replace("Monospace", "Courier New");
      }
    }
    if (line.contains("<font_size>")) { // defaults: linux: 12, macOS: 16
      if (QSysInfo::productType().contains("osx")) {
        line.replace("12", "16");
      }
    }
    fout << line << Qt::endl;
  }

  file.close();
  fres.close();
}

//--------------------------------------------------------------------------
// END
//--------------------------------------------------------------------------
