/****************************************************************************

  PChangeDefaultPathsDialog.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

*****************************************************************************/

/***************************************************************************
 *   Copyright (C) 2010-2019 by Andreas Suter                              *
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

#include <QString>
#include <QStringRef>
#include <QMessageBox>
#include <QFileDialog>
#include <QListWidgetItem>
#include <QIODevice>
#include <QFile>
#include <QProcessEnvironment>
#include <QTextStream>

#include "PChangeDefaultPathsDialog.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * <p>XML Parser class for the musrfit administration file.
 *
 * \param fln file name of the musredit startup xml-file
 * @param defaultPaths
 */
PDefaultPathsXMLParser::PDefaultPathsXMLParser(const QString& fln, PDefaultPaths *defaultPaths) :
  fDefaultPaths(defaultPaths)
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
 * <p>parse the musrfit startup xml-file.
 *
 * \param device QFile object of the musrfit startup xml-file
 *
 * @return true on success, false otherwise
 */
bool PDefaultPathsXMLParser::parse(QIODevice *device)
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
bool PDefaultPathsXMLParser::startDocument()
{
  // nothing to be done here for now
  return true;
}

//--------------------------------------------------------------------------
/**
 * <p>Routine called when a new XML tag is found. Here it is used
 * to set a tag for filtering afterwards the content.
 */
bool PDefaultPathsXMLParser::startElement()
{
  QStringRef qName = fXml.name();

  if (qName == "data_path") {
    fKeyWord = eDataPath;
  }

  return true;
}

//--------------------------------------------------------------------------
/**
 * <p>Routine called when the end XML tag is found. It is used to
 * put the filtering tag to 'empty'.
 */
bool PDefaultPathsXMLParser::endElement()
{
  fKeyWord = eEmpty;

  return true;
}

//--------------------------------------------------------------------------
/**
 * <p>This routine delivers the content of an XML tag. It fills the
 * content into the load data structure.
 */
bool PDefaultPathsXMLParser::characters()
{
  QString str = *fXml.text().string();
  if (str.isEmpty())
    return true;

  switch (fKeyWord) {
    case eDataPath:
      fDefaultPaths->appendDefaultPath(str);
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
bool PDefaultPathsXMLParser::endDocument()
{
  return true;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * @brief PDefaultPaths::PDefaultPaths
 */
PDefaultPaths::PDefaultPaths() : QObject()
{
  fValid = true;

  // XML Parser part
  // 1st: check local directory
  QString path = QString("./");
  QString fln = QString("musrfit_startup.xml");
  QString pathFln = path + fln;
  QProcessEnvironment procEnv = QProcessEnvironment::systemEnvironment();
  if (!QFile::exists(pathFln)) {
    // 2nd: check $HOME/.musrfit/musrfit_startup.xml
    path = procEnv.value("HOME", "");
    pathFln = path + "/.musrfit/" + fln;
    if (!QFile::exists(pathFln)) {
      // 3rd: check $MUSRFITPATH/musrfit_startup.xml
      path = procEnv.value("MUSRFITPATH", "");
      pathFln = path + "/" + fln;
      if (!QFile::exists(pathFln)) {
        // 4th: check $ROOTSYS/bin/musrfit_startup.xml
        path = procEnv.value("ROOTSYS", "");
        pathFln = path + "/bin/" + fln;
      }
    }
  }
  fPrefPathName = pathFln;

  // read musrfit_startup.xml and extract default data file search paths
  PDefaultPathsXMLParser handler(fPrefPathName, this);
  if (!handler.isValid()) {
    fValid = false;
    QMessageBox::critical(0, "ERROR",
                          "Error parsing musrfit_startup.xml settings file.\nProbably a few things will not work porperly.\nPlease fix this first.",
                          QMessageBox::Ok, QMessageBox::NoButton);
    return;
  }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * <p>Constructor.
 *
 * \param fAdmin keeps all the needed flags
 */
PChangeDefaultPathsDialog::PChangeDefaultPathsDialog()
{
  fDefaultPath = 0;
  fDefaultPath = new PDefaultPaths();
  if (!fDefaultPath->isValid())
    return;

  QStringList *strList = fDefaultPath->getDefaultPathList();
  if (strList == 0)
    return;

  setupUi(this);

  setModal(true);

  // populate default search paths
  for (int i=0; i<strList->count(); i++) {
    fSearchPath_listWidget->addItem(strList->at(i));
  }

  QObject::connect(fDelete_pushButton, SIGNAL(clicked()), this, SLOT(deleteItem()));
  QObject::connect(fAdd_pushButton, SIGNAL(clicked()), this, SLOT(addItem()));
  QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(saveDefaultPathList()));
}

//----------------------------------------------------------------------------------------------------
/**
 * @brief PChangeDefaultPathsDialog::addItem
 */
void PChangeDefaultPathsDialog::addItem()
{
  // call QFileDialog in order to get the proper directory
  QString dirName = QFileDialog::getExistingDirectory(this, tr("Data File Directory"), "",
                                             QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
  if (dirName == "")
    return;

  // insert the new directory in fSearchPath_listWidget
  QListWidgetItem *item = new QListWidgetItem;
  if (item == 0) {
    QMessageBox::critical(this, "ERROR", "Couldn't invoke QListWidgetItem! Won't do anything.");
    return;
  }
  item->setText(dirName);
  fSearchPath_listWidget->insertItem(fSearchPath_listWidget->currentRow()+1, item);
}

//----------------------------------------------------------------------------------------------------
/**
 * @brief PChangeDefaultPathsDialog::deleteItem
 */
void PChangeDefaultPathsDialog::deleteItem()
{
  QListWidgetItem *item = fSearchPath_listWidget->takeItem(fSearchPath_listWidget->currentRow());
  if (item != 0)
    delete item;
}

//----------------------------------------------------------------------------------------------------
/**
 * @brief PChangeDefaultPathsDialog::saveDefaultPathList
 */
void PChangeDefaultPathsDialog::saveDefaultPathList()
{
  // read the used musrfit_startup.xml
  QFile fileIn(fDefaultPath->getPrefPathName());
  if (!fileIn.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QMessageBox::critical(this, "ERROR", QString("Cannot read '%1'. Won't do anything.").arg(fDefaultPath->getPrefPathName()));
    return;
  }
  QTextStream in(&fileIn);
  QStringList fileContent;
  while (!in.atEnd()) {
    fileContent << in.readLine();
  }
  fileIn.close();

  // check if there is any data_path is present in the musrfit_startup.xml
  bool dataPathPresent = false;
  QString str;
  for (int i=0; i<fileContent.count(); i++) {
    str = fileContent[i];
    if (str.trimmed().startsWith("<data_path>")) {
      dataPathPresent = true;
      break;
    }
  }

  // write the new musrfit_startup.xml
  QFile fileOut(fDefaultPath->getPrefPathName());
  if (!fileOut.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QMessageBox::critical(this, "ERROR", QString("Cannot write to '%1'.").arg(fDefaultPath->getPrefPathName()));
    return;
  }
  QTextStream out(&fileOut);
  bool first = true;
  for (int i=0; i<fileContent.count(); i++) {
    str = fileContent[i];
    if (!str.trimmed().startsWith("<data_path>")) {
      // if not data_path was present, add the new data_paths just before the end of the musrfit_start.xml close tag
      if ((dataPathPresent == false) && (str.trimmed().startsWith("</musrfit>"))) {
        for (int j=0; j<fSearchPath_listWidget->count(); j++) {
#if (QT_VERSION < QT_VERSION_CHECK(5, 14, 0))
          out << "    <data_path>" << fSearchPath_listWidget->item(j)->text() << "</data_path>" << endl;
#else
          out << "    <data_path>" << fSearchPath_listWidget->item(j)->text() << "</data_path>" << Qt::endl;
#endif
        }
      }
#if (QT_VERSION < QT_VERSION_CHECK(5, 14, 0))
      out << fileContent[i] << endl;
#else
      out << fileContent[i] << Qt::endl;
#endif
    } else {
      if (first) {
        first = false;
        for (int j=0; j<fSearchPath_listWidget->count(); j++)
#if (QT_VERSION < QT_VERSION_CHECK(5, 14, 0))
          out << "    <data_path>" << fSearchPath_listWidget->item(j)->text() << "</data_path>" << endl;
#else
          out << "    <data_path>" << fSearchPath_listWidget->item(j)->text() << "</data_path>" << Qt::endl;
#endif
      }
    }
  }
  fileOut.close();
}

//----------------------------------------------------------------------------------------------------
// END
//----------------------------------------------------------------------------------------------------
