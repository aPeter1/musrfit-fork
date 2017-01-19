/****************************************************************************

  PChangeDefaultPathsDialog.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

*****************************************************************************/

/***************************************************************************
 *   Copyright (C) 2010-2017 by Andreas Suter                              *
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

#include <QMessageBox>
#include <QFileDialog>
#include <QListWidgetItem>

#include "PChangeDefaultPathsDialog.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * @brief PDefaultPathsXMLParser::PDefaultPathsXMLParser
 * @param defaultPaths
 */
PDefaultPathsXMLParser::PDefaultPathsXMLParser(PDefaultPaths *defaultPaths) :
  fDefaultPaths(defaultPaths)
{
  fKeyWord = eEmpty;
}

//--------------------------------------------------------------------------
/**
 * <p>Routine called at the beginning of the XML parsing process.
 */
bool PDefaultPathsXMLParser::startDocument()
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
bool PDefaultPathsXMLParser::startElement( const QString&, const QString&,
                                           const QString& qName,
                                           const QXmlAttributes& )
{
  if (qName == "data_path") {
    fKeyWord = eDataPath;
  }

  return true;
}

//--------------------------------------------------------------------------
/**
 * <p>Routine called when the end XML tag is found. It is used to
 * put the filtering tag to 'empty'.
 *
 * \param qName name of the element.
 */
bool PDefaultPathsXMLParser::endElement( const QString&, const QString&, const QString & )
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
bool PDefaultPathsXMLParser::characters(const QString& str)
{
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

//--------------------------------------------------------------------------
/**
 * <p>Report XML warnings.
 *
 * \param exception holds the information of the XML warning
 */
bool PDefaultPathsXMLParser::warning( const QXmlParseException & exception )
{
  QString msg;

  msg  = QString("**WARNING** while parsing musrfit_startup.xml in line no %1\n").arg(exception.lineNumber());
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
bool PDefaultPathsXMLParser::error( const QXmlParseException & exception )
{
  QString msg;

  msg  = QString("**ERROR** while parsing musrfit_startup.xml in line no %1\n").arg(exception.lineNumber());
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
bool PDefaultPathsXMLParser::fatalError( const QXmlParseException & exception )
{
  QString msg;

  msg  = QString("**FATAL ERROR** while parsing musrfit_startup.xml in line no %1\n").arg(exception.lineNumber());
  msg += QString("**FATAL ERROR MESSAGE** ") + exception.message();

  qWarning() << endl << msg << endl;

  QMessageBox::critical(0, "FATAL ERROR", msg, QMessageBox::Ok, QMessageBox::NoButton);

  return true;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
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
  PDefaultPathsXMLParser handler(this);
  QFile xmlFile(fPrefPathName);
  QXmlInputSource source( &xmlFile );
  QXmlSimpleReader reader;
  reader.setContentHandler( &handler );
  reader.setErrorHandler( &handler );
  if (!reader.parse( source )) {
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

  // write the new musrfit_startup.xml
  QFile fileOut(fDefaultPath->getPrefPathName());
  if (!fileOut.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QMessageBox::critical(this, "ERROR", QString("Cannot write to '%1'.").arg(fDefaultPath->getPrefPathName()));
    return;
  }
  QTextStream out(&fileOut);
  QString str;
  bool first = true;
  for (int i=0; i<fileContent.count(); i++) {
    str = fileContent[i];
    if (!str.trimmed().startsWith("<data_path>")) {
      out << fileContent[i] << endl;
    } else {
      if (first) {
        first = false;
        for (int j=0; j<fSearchPath_listWidget->count(); j++)
          out << "    <data_path>" << fSearchPath_listWidget->item(j)->text() << "</data_path>" << endl;
      }
    }
  }
  fileOut.close();
}

//----------------------------------------------------------------------------------------------------
// END
//----------------------------------------------------------------------------------------------------
