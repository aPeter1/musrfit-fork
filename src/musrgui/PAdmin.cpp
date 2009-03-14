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
  } else if (qName == "msr_default_file_path") {
    fKeyWord = eMsrDefaultFilePath;
  }

  return true;
}

//--------------------------------------------------------------------------
/**
 * <p>Routine called when the end XML tag is found. It is used to
 * put the filtering tag to 'empty'.
 */
bool PAdminXMLParser::endElement( const QString&, const QString&, const QString& )
{
  fKeyWord = eEmpty;

  return true;
}

//--------------------------------------------------------------------------
/**
 * <p>This routine delivers the content of an XML tag. It fills the
 * content into the load data structure.
 */
bool PAdminXMLParser::characters(const QString& str)
{
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
    case eMsrDefaultFilePath:
      fAdmin->setMsrDefaultFilePath(QString(str.ascii()).stripWhiteSpace());
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

  fBeamline   = QString("");
  fInstitute  = QString("");
  fFileFormat = QString("");

  // XML Parser part
  QString path = getenv("MUSRFITPATH");
  if (path.isEmpty())
    path = "/home/nemu/analysis/bin";
  QString fln = path + "/musrgui_startup.xml";
  if (QFile::exists(fln)) { // administrations file present
    PAdminXMLParser handler(this);
    QFile xmlFile(fln);
    QXmlInputSource source( &xmlFile );
    QXmlSimpleReader reader;
    reader.setContentHandler( &handler );
    reader.parse( source );
  }
}

//--------------------------------------------------------------------------
// END
//--------------------------------------------------------------------------
