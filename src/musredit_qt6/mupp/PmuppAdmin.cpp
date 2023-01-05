/****************************************************************************

  PmuppAdmin.cpp

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

#include <QMessageBox>
#include <QString>
#include <QStringView>
#include <QFile>
#include <QTextStream>
#include <QVector>
#include <QDir>
#include <QProcessEnvironment>
#include <QSysInfo>

#include <QtDebug>

#include "PmuppAdmin.h"

//--------------------------------------------------------------------------
// implementation of PmuppColor class
//--------------------------------------------------------------------------
/**
 * @brief PmuppColor::PmuppColor. Ctor
 */
PmuppColor::PmuppColor()
{
  fName  = "UnDef";
  fRed   = -1;
  fGreen = -1;
  fBlue  = -1;
}

//--------------------------------------------------------------------------
/**
 * @brief PmuppColor::setRGB. set RGB value
 * @param r red (0..255)
 * @param g green (0..255)
 * @param b blue (0..255)
 */
void PmuppColor::setRGB(const int r, const int g, const int b)
{
  if ((r>=0) && (r<=255))
    fRed = r;
  if ((g>=0) && (g<=255))
    fGreen = g;
  if ((b>=0) && (b<=255))
    fBlue = b;
}

//--------------------------------------------------------------------------
// implementation of PmuppAdminXMLParser class
//--------------------------------------------------------------------------
/**
 * <p>XML Parser class for the mupp administration file.
 *
 * \param admin pointer to an admin class instance.
 */
PmuppAdminXMLParser::PmuppAdminXMLParser(const QString& fln, PmuppAdmin *admin) : fAdmin(admin)
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
 * <p>parse the mupp startup xml-file.
 *
 * \param device QFile object of the mupp startup xml-file
 *
 * @return true on success, false otherwise
 */
bool PmuppAdminXMLParser::parse(QIODevice *device)
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
    QMessageBox::critical(0, "**ERROR**", msg, QMessageBox::Ok, QMessageBox::NoButton);
    return false;
  }

  return true;
}

//--------------------------------------------------------------------------
/**
 * <p>Routine called at the beginning of the XML parsing process.
 */
bool PmuppAdminXMLParser::startDocument()
{
  // nothing to be done here for now
  return true;
}

//--------------------------------------------------------------------------
/**
 * <p>Routine called when a new XML tag is found. Here it is used
 * to set a tag for filtering afterwards the content.
 */
bool PmuppAdminXMLParser::startElement()
{
  QString qName = fXml.name().toString();

  if (qName == "path_file_name") {
    fKeyWord = eRecentFile;
  } else if (qName == "dark_theme") {
    fKeyWord = eDarkTheme;
  } else if (qName == "marker") {
    fKeyWord = eMarker;
  } else if (qName == "color") {
    fKeyWord = eColor;
  }

  return true;
}

//--------------------------------------------------------------------------
/**
 * <p>Routine called when the end XML tag is found. It is used to
 * put the filtering tag to 'empty'. It also resets the fFunc flag in case
 * the entry was a theory function.
 */
bool PmuppAdminXMLParser::endElement()
{
  fKeyWord = eEmpty;

  return true;
}

//--------------------------------------------------------------------------
/**
 * <p>This routine delivers the content of an XML tag. It fills the
 * content into the load data structure.
 */
bool PmuppAdminXMLParser::characters()
{
  QString str = fXml.text().toString();
  if (str.isEmpty())
    return true;

  bool ok;
  int  ival, r, g, b;
  double dval;
  QString name("");
  QStringList tok;

  switch (fKeyWord) {
    case eRecentFile:
      fAdmin->addRecentFile(QString(str.toLatin1()).trimmed());
      break;
    case eDarkTheme:
      if ((str == "yes") || (str == "1") || (str == "true"))
        fAdmin->setTheme(true);
      else
        fAdmin->setTheme(false);
      break;
    case eMarker:
      tok = str.split(",", Qt::SkipEmptyParts);

      if ((tok.count() != 1) && (tok.count() != 2)) {
        return false;
      }

      ival = tok[0].toInt(&ok);
      if (!ok)
        return false;

      dval = 1.0;
      if (tok.count() == 2) {
        dval = tok[1].toDouble(&ok);
        if (!ok)
          return false;
      }
      fAdmin->setMarker(ival, dval);
      break;
    case eColor:
      tok = str.split(",", Qt::SkipEmptyParts);

      if ((tok.count() != 3) && (tok.count() != 4)) {
        return false;
      }
      ival = tok[0].toInt(&ok);
      if (!ok)
        return false;
      r = ival;
      ival = tok[1].toInt(&ok);
      if (!ok)
        return false;
      g = ival;
      ival = tok[2].toInt(&ok);
      if (!ok)
        return false;
      b = ival;
      if (tok.count() == 4)
        name = tok[3];
      fAdmin->setColor(r, g, b, name);
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
bool PmuppAdminXMLParser::endDocument()
{
  return true;
}

//--------------------------------------------------------------------------
// implementation of PmuppAdmin class
//--------------------------------------------------------------------------
/**
 * <p>Initializes that PmuppAdmin object, and calls the XML parser which feeds
 * the object variables.
 */
PmuppAdmin::PmuppAdmin() : QObject(), fDarkTheme(false)
{
  // XML Parser part
  // 1st: check local directory
  QString path = QString("./");
  QString fln = QString("mupp_startup.xml");
  QString pathFln = path + fln;
  QProcessEnvironment procEnv = QProcessEnvironment::systemEnvironment();
  if (!QFile::exists(pathFln)) {
    // 2nd: check $HOME/.musrfit/mupp/mupp_startup.xml
    path = procEnv.value("HOME", "");
    pathFln = path + "/.musrfit/mupp/" + fln;
    if (!QFile::exists(pathFln)) {
      // 3rd: check $MUSRFITPATH/mupp_startup.xml
      path = procEnv.value("MUSRFITPATH", "");
      pathFln = path + "/" + fln;
      if (!QFile::exists(pathFln)) {
        // 4th: check $ROOTSYS/bin/mupp_startup.xml
        path = procEnv.value("ROOTSYS", "");
        pathFln = path + "/bin/" + fln;
        if (!QFile::exists(pathFln)) {
          // 5th: not found anywhere hence create it
          path = procEnv.value("HOME", "");
          pathFln = path + "/.musrfit/mupp/" + fln;
          createMuppStartupFile();
        }
      }
    }
  }

  if (QFile::exists(pathFln)) { // administration file present    
    PmuppAdminXMLParser handler(pathFln, this);
    if (!handler.isValid()) {
      QMessageBox::critical(0, "**ERROR**",
                            "Error parsing mupp_startup.xml settings file.\nProbably a few things will not work porperly.\nPlease fix this first.",
                            QMessageBox::Ok, QMessageBox::NoButton);
      return;
    }
  } else {
    QMessageBox::critical(0, "**ERROR**",
                          "Couldn't find the mupp_startup.xml settings file.\nProbably a few things will not work porperly.\nPlease fix this first.",
                          QMessageBox::Ok, QMessageBox::NoButton);
    return;
  }
}

//--------------------------------------------------------------------------
/**
 * <p>Destructor
 */
PmuppAdmin::~PmuppAdmin()
{
  saveRecentFiles();
}

//--------------------------------------------------------------------------
/**
 * <p>Add recent path-file name to the internal ring-buffer.
 *
 * \param str recent path-file name to be added
 */
void PmuppAdmin::addRecentFile(const QString str)
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
 * @brief PmuppAdmin::getRecentFile. Get a file from the recent files with
 * index idx.
 * @param idx
 *
 * @return return the recent file with index idx if present, otherwise return
 * an empty string.
 */
QString PmuppAdmin::getRecentFile(int idx)
{
  QString str("");

  if ((idx >= 0) && (idx < fRecentFile.size()))
    str = fRecentFile[idx];

  return str;
}

//--------------------------------------------------------------------------
/**
 * @brief PmuppAdmin::getMarker. Get the marker with index idx.
 * @param idx marker index
 *
 * @return requested marker
 */
PmuppMarker PmuppAdmin::getMarker(int idx) {
  PmuppMarker marker;

  if (idx >= fMarker.size())
    return marker;

  return fMarker[idx];
}

//--------------------------------------------------------------------------
/**
 * @brief PmuppAdmin::getColor. Get rgb color codes of name
 * @param name of the requeset color.
 * @param r red value (0..255)
 * @param g green value (0..255)
 * @param b blue value (0..255
 */
void PmuppAdmin::getColor(QString name, int &r, int &g, int &b)
{
  int idx=-1;
  for (int i=0; i<fColor.size(); i++) {
    if (fColor[i].getName() == name) {
      idx = i;
      break;
    }
  }

  if (idx == -1) {
    r = -1;
    g = -1;
    b = -1;
  } else {
    fColor[idx].getRGB(r, g, b);
  }
}

//--------------------------------------------------------------------------
/**
 * @brief PmuppAdmin::getColor. Get color codes of color at index idx
 * @param idx requested color index
 * @param r red value (0..255)
 * @param g green value (0..255)
 * @param b blue value (0..255)
 */
void PmuppAdmin::getColor(int idx, int &r, int &g, int &b)
{
  if ((idx<=0) || (idx>fColor.size())) {
    r = -1;
    g = -1;
    b = -1;
  } else {
    fColor[idx].getRGB(r, g, b);
  }
}

//--------------------------------------------------------------------------
/**
 * @brief PmuppAdmin::setMaker. Set a marker
 * @param marker marker code
 * @param size marker size
 */
void PmuppAdmin::setMarker(int marker, double size)
{
  PmuppMarker markerObj;

  // make sure marker is in proper range
  if ((marker<1) || (marker>49)) {
    QMessageBox::warning(0, "WARNING", QString("Found Marker (%1) not in the expected range.\nWill ignore it.").arg(marker));
    return;
  }
  markerObj.setMarker(marker);
  markerObj.setMarkerSize(size);

  fMarker.push_back(markerObj);
}

//--------------------------------------------------------------------------
/**
 * @brief PmuppAdmin::setColor. Set a color
 * @param r red value (0..255)
 * @param g green value (0..255)
 * @param b blue value (0..255)
 * @param name color name
 */
void PmuppAdmin::setColor(int r, int g, int b, QString name)
{
  if (((r<0) || (r>255)) ||
      ((g<0) || (g>255)) ||
      ((b<0) || (b>255))) {
    QMessageBox::warning(0, "WARNING", QString("Found Color (%1,%2,%3) not in the expected range.\nWill ignore it.").arg(r).arg(g).arg(b));
    return;
  }

  PmuppColor color;
  color.setName(name);
  color.setRGB(r,g,b);

  fColor.push_back(color);
}

//--------------------------------------------------------------------------
/**
 * <p>Merges the recent file ring buffer into mupp_startup.xml and saves it.
 * If a local copy is present it will be saved there, otherwise the master file
 * will be used.
 */
void PmuppAdmin::saveRecentFiles()
{
  // check if mupp_startup.xml is present in the current directory, and if yes, use this file to
  // save the recent file names otherwise use the "master" mupp_startup.xml

  QString str("");
  QString fln = QString("./mupp_startup.xml");
  if (!QFile::exists(fln)) {
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    fln = QString("%1/.musrfit/mupp/mupp_startup.xml").arg(env.value("HOME"));
  }

  if (QFile::exists(fln)) { // administration file present
    QVector<QString> data;
    QFile file(fln);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
      std::cerr << std::endl << ">> PmuppAdmin::saveRecentFile: **ERROR** Cannot open " << fln.toLatin1().data() << " for reading." << std::endl;
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
      std::cerr << std::endl << ">> PmuppAdmin::saveRecentFile: **ERROR** " << fln.toLatin1().data() << " seems to be corrupt." << std::endl;
      return;
    }
    i++;
    for (int j=0; j<fRecentFile.size(); j++) {
      str = "    <path_file_name>" + fRecentFile[j] + "</path_file_name>";
      data.insert(i++, str);
    }

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
      std::cerr << std::endl << ">> PmuppAdmin::saveRecentFile: **ERROR** Cannot open " << fln.toLatin1().data() << " for reading." << std::endl;
      return;
    }
    fin.setDevice(&file);
    for (int i=0; i<data.size(); i++) {
      fin << data[i] << Qt::endl;
    }
    file.close();
  } else {
    QString msg("Failed to write mupp_startup.xml. Neither a local nor a global copy found.");
    QMessageBox::warning(0, "WARNING", msg, QMessageBox::Ok, QMessageBox::NoButton);
  }
}

//--------------------------------------------------------------------------
/**
 * @brief PmuppAdmin::createMuppStartupFile. Create a default mupp_startup.xml
 * file if not present.
 */
void PmuppAdmin::createMuppStartupFile()
{
  // get $HOME
  QProcessEnvironment procEnv = QProcessEnvironment::systemEnvironment();
  QString pathName = procEnv.value("HOME", "");
  pathName += "/.musrfit/mupp";

  // check if the directory $HOME/.musrfit/mupp exists if not create it
  QDir dir(pathName);
  if (!dir.exists()) {
    // directory $HOME/.musrfit/mupp does not exist hence create it
    dir.mkpath(pathName);
  }

  // create default mupp_startup.xml file in $HOME/.musrfit/mupp
  pathName += "/mupp_startup.xml";

  // get the default mupp_startup.xml.in from the internal resources
  QFile fres(":/mupp_startup.xml.in");
  if (!fres.exists()) {
    QString msg = QString("Neither couldn't find nor create mupp_startup.xml. Things are likely not to work.");
    QMessageBox::critical(0, "ERROR", msg);
    return;
  }

  if (!fres.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QString msg = QString("Couldn't open internal resource file mupp_startup.xml.in. Things are likely not to work.");
    QMessageBox::critical(0, "ERROR", msg);
    return;
  }
  // text stream for fres
  QTextStream fin(&fres);

  // mupp_startup.xml default file
  QFile file(pathName);

  if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    return;

  // text stream for file
  QTextStream fout(&file);

  QString line;
  while (!fin.atEnd()) {
    line = fin.readLine();
    fout << line << Qt::endl;
  }

  file.close();
  fres.close();
}

//--------------------------------------------------------------------------
// END
//--------------------------------------------------------------------------
