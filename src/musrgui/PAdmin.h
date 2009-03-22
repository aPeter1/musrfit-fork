/****************************************************************************

  PAdmin.h

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

#ifndef _PADMIN_H_
#define _PADMIN_H_

#include <qstring.h>
#include <qvaluevector.h>
#include <qpixmap.h>
#include <qxml.h>

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
    enum EAdminKeyWords {eEmpty, eExecPath, eDefaultSavePath, eBeamline, eInstitute, eFileFormat,
                         eLifetimeCorrection, eMsrDefaultFilePath, eHelpMain, eTheoFuncPixmapPath,
                         eFunc, eFuncName, eFuncComment, eFuncLabel, eFuncPixmap, eFuncParams};

    bool startDocument();
    bool startElement( const QString&, const QString&, const QString& ,
                       const QXmlAttributes& );
    bool endElement( const QString&, const QString&, const QString& );

    bool characters(const QString&);
    bool endDocument();

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

    QString getExecPath() { return fExecPath; }
    QString getDefaultSavePath() { return fDefaultSavePath; }
    QString getBeamline() { return fBeamline; }
    QString getInstitute() { return fInstitute; }
    QString getFileFormat() { return fFileFormat; }
    bool    getLifetimeCorrectionFlag() { return fLifetimeCorrection; }
    QString getMsrDefaultFilePath() { return fMsrDefaultFilePath; }
    QString getHelpMain() { return fHelpMain; }
    QString getTheoFuncPixmapPath() { return fTheoFuncPixmapPath; }
    unsigned int getTheoryCounts() { return fTheory.size(); }
    PTheory* getTheoryItem(const unsigned int idx);

  protected:
    void setExecPath(const QString str) { fExecPath = str; }
    void setDefaultSavePath(const QString str) { fDefaultSavePath = str; }
    void setBeamline(const QString str) { fBeamline = str; }
    void setInstitute(const QString str) { fInstitute = str; }
    void setFileFormat(const QString str) { fFileFormat = str; }
    void setLifetimeCorrectionFlag(const bool flag) { fLifetimeCorrection = flag; }
    void setMsrDefaultFilePath(const QString str) { fMsrDefaultFilePath = str; }
    void setHelpMain(const QString str) { fHelpMain = str; }
    void setTheoFuncPixmapPath (const QString str) { fTheoFuncPixmapPath = str; }
    void addTheoryItem(const PTheory theo) { fTheory.push_back(theo); }

  private:
    friend class PAdminXMLParser;

    QString fExecPath;
    QString fDefaultSavePath;
    QString fMsrDefaultFilePath;
    QString fTheoFuncPixmapPath;

    QString fBeamline;
    QString fInstitute;
    QString fFileFormat;
    bool fLifetimeCorrection;

    QString fHelpMain;

    QValueVector<PTheory> fTheory;
};

#endif // _PADMIN_H_
