/****************************************************************************

  PAdmin.h

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

#ifndef _PADMIN_H_
#define _PADMIN_H_

#include <QString>
#include <QVector>
#include <QMap>
#include <QPixmap>
#include <QtXml>

class PAdmin;

//---------------------------------------------------------------------------
typedef struct {
  QString name;
  QString comment;
  QString label;
  QString pixmapName;
  QPixmap pixmap;
  int     params;
} PTheory;

//---------------------------------------------------------------------------
class PAdminXMLParser : public QXmlDefaultHandler
{
  public:
    PAdminXMLParser(PAdmin*);
    virtual ~PAdminXMLParser() {}

  private:
    enum EAdminKeyWords {eEmpty, eFontName, eFontSize, eExecPath, eDefaultSavePath, eTitleFromDataFile, eEnableMusrT0,
                         eBeamline, eInstitute, eFileFormat, eLifetimeCorrection, eMsrDefaultFilePath,
                         eTheoFuncPixmapPath, eFunc, eFuncName, eFuncComment, eFuncLabel,
                         eFuncPixmap, eFuncParams, eHelpMain, eHelpTitle, eHelpParameters, eHelpTheory, eHelpFunctions,
                         eHelpRun, eHelpCommand, eHelpFourier, eHelpPlot, eHelpStatistic};

    bool startDocument();
    bool startElement( const QString&, const QString&, const QString& ,
                       const QXmlAttributes& );
    bool endElement( const QString&, const QString&, const QString& );

    bool characters(const QString&);
    bool endDocument();

    QString expandPath(const QString&);

    EAdminKeyWords fKeyWord;
    bool           fFunc;
    PTheory        fTheoryItem;
    PAdmin         *fAdmin;
};

//---------------------------------------------------------------------------
class PAdmin
{
  public:
    PAdmin();
    virtual ~PAdmin() {}

    QString getFontName() { return fFontName; }
    int     getFontSize() { return fFontSize; }
    QString getExecPath() { return fExecPath; }
    QString getDefaultSavePath() { return fDefaultSavePath; }
    bool    getTitleFromDataFileFlag() { return fTitleFromDataFile; }
    bool    getEnableMusrT0Flag() { return fEnableMusrT0; }
    QString getBeamline() { return fBeamline; }
    QString getInstitute() { return fInstitute; }
    QString getFileFormat() { return fFileFormat; }
    bool    getLifetimeCorrectionFlag() { return fLifetimeCorrection; }
    QString getMsrDefaultFilePath() { return fMsrDefaultFilePath; }
    QString getHelpUrl(QString tag);
    QString getTheoFuncPixmapPath() { return fTheoFuncPixmapPath; }
    unsigned int getTheoryCounts() { return fTheory.size(); }
    PTheory* getTheoryItem(const unsigned int idx);

    void setFontName(const QString str) { fFontName = str; }
    void setFontSize(const int ival) { fFontSize = ival; }

  protected:
    void setExecPath(const QString str) { fExecPath = str; }
    void setDefaultSavePath(const QString str) { fDefaultSavePath = str; }
    void setTitleFromDataFileFlag(const bool flag) { fTitleFromDataFile = flag; }
    void setEnableMusrT0Flag(const bool flag) { fEnableMusrT0 = flag; }
    void setBeamline(const QString str) { fBeamline = str; }
    void setInstitute(const QString str) { fInstitute = str; }
    void setFileFormat(const QString str) { fFileFormat = str; }
    void setLifetimeCorrectionFlag(const bool flag) { fLifetimeCorrection = flag; }
    void setMsrDefaultFilePath(const QString str) { fMsrDefaultFilePath = str; }
    void setHelpUrl(const QString tag, const QString url);
    void setTheoFuncPixmapPath (const QString str) { fTheoFuncPixmapPath = str; }
    void addTheoryItem(const PTheory theo) { fTheory.push_back(theo); }

  private:
    friend class PAdminXMLParser;

    QString fFontName;
    int fFontSize;

    QString fExecPath;
    QString fDefaultSavePath;
    QString fMsrDefaultFilePath;
    QString fTheoFuncPixmapPath;

    bool fTitleFromDataFile;
    bool fEnableMusrT0;

    QString fBeamline;
    QString fInstitute;
    QString fFileFormat;
    bool fLifetimeCorrection;

    QMap<QString, QString> fHelpUrl; ///< maps tag to help url

    QVector<PTheory> fTheory;
};

#endif // _PADMIN_H_
