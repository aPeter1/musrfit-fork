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
  if (qName == "exec_path") {
    fKeyWord = eExecPath;
  } else if (qName == "default_save_path") {
    fKeyWord = eDefaultSavePath;
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

  switch (fKeyWord) {
    case eExecPath:
      fAdmin->setExecPath(QString(str.ascii()).stripWhiteSpace());
      break;
    case eDefaultSavePath:
      fAdmin->setDefaultSavePath(QString(str.ascii()).stripWhiteSpace());
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
      bool flag;
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

  return true;
}

//--------------------------------------------------------------------------
// implementation of PAdmin class
//--------------------------------------------------------------------------
/**
 * <p>
 */
PAdmin::PAdmin()
{
  fExecPath = QString("");
  fDefaultSavePath = QString("");
  fMsrDefaultFilePath = QString("");
  fTheoFuncPixmapPath = QString("");

  fBeamline   = QString("");
  fInstitute  = QString("");
  fFileFormat = QString("");

  fHelpMain   = QString("");

  // XML Parser part
  QString fln = "./musrgui_startup.xml";
  if (!QFile::exists(fln)) {
    QString path = std::getenv("MUSRFITPATH");
    QString home = std::getenv("HOME");
    if (path.isEmpty())
      path = home + "/analysis/bin";
    fln = path + "/musrgui_startup.xml";
  }
  if (QFile::exists(fln)) { // administration file present
    PAdminXMLParser handler(this);
    QFile xmlFile(fln);
    QXmlInputSource source( &xmlFile );
    QXmlSimpleReader reader;
    reader.setContentHandler( &handler );
    reader.parse( source );
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
