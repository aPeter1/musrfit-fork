/****************************************************************************

  PChangeDefaultPathsDialog.h

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

#ifndef _PCHANGEDEFAULTPATHSDIALOG_H_
#define _PCHANGEDEFAULTPATHSDIALOG_H_

#include <QDialog>
#include <QMessageBox>
#include <QXmlStreamReader>
#include <QIODevice>

#include "ui_PChangeDefaultPathsDialog.h"

class PDefaultPaths;

//---------------------------------------------------------------------------
class PDefaultPathsXMLParser
{
  public:
    PDefaultPathsXMLParser(const QString &fln, PDefaultPaths *defaultPaths);
    virtual ~PDefaultPathsXMLParser() {}

    virtual bool isValid() { return fValid; }

  private:
    enum EAdminKeyWords {eEmpty, eDataPath};

    bool parse(QIODevice *device);
    bool startDocument();
    bool startElement();
    bool endElement();
    bool characters();
    bool endDocument();

    QXmlStreamReader fXml;        ///< xml stream reader object
    bool             fValid;      ///< flag showing if XML read has been successful
    EAdminKeyWords   fKeyWord;    ///< key word tag to know how to handle the content
    PDefaultPaths    *fDefaultPaths; ///< keeps the default search paths for the data files
};

//---------------------------------------------------------------------------
class PDefaultPaths : public QObject
{
  public:
    PDefaultPaths();
    virtual ~PDefaultPaths() {}

    virtual bool isValid() { return fValid; }
    virtual void appendDefaultPath(QString str) { fDefaultPath << str; }
    virtual QStringList *getDefaultPathList() { return &fDefaultPath; }
    virtual QString getPrefPathName() { return fPrefPathName; }

  private:
    friend class PDefaultPathsXMLParser;

    bool fValid;
    QString fPrefPathName;
    QStringList fDefaultPath;
};

//---------------------------------------------------------------------------
class PChangeDefaultPathsDialog : public QDialog, private Ui::PChangeDefaultPathsDialog
{
  Q_OBJECT

  public:
    PChangeDefaultPathsDialog();

  private slots:
    void deleteItem();
    void addItem();
    void saveDefaultPathList();

  private:
    PDefaultPaths *fDefaultPath;
};

#endif // _PCHANGEDEFAULTPATHSDIALOG_H_
