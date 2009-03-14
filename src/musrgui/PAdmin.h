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
#include <qptrlist.h>
#include <qxml.h>

class PAdmin;

//---------------------------------------------------------------------------
class PAdminXMLParser : public QXmlDefaultHandler
{
  public:
    PAdminXMLParser(PAdmin*);
    virtual ~PAdminXMLParser() {}

  private:
    enum EAdminKeyWords {eEmpty, eExecPath, eDefaultSavePath, eBeamline, eInstitute, eFileFormat,
                         eMsrDefaultFilePath};

    bool startDocument();
    bool startElement( const QString&, const QString&, const QString& ,
                       const QXmlAttributes& );
    bool endElement( const QString&, const QString&, const QString& );

    bool characters(const QString&);
    bool endDocument();

    EAdminKeyWords fKeyWord;
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
    QString getMsrDefaultFilePath() { return fMsrDefaultFilePath; }

  protected:
    void setExecPath(const QString str) { fExecPath = str; }
    void setDefaultSavePath(const QString str) { fDefaultSavePath = str; }
    void setBeamline(const QString str) { fBeamline = str; }
    void setInstitute(const QString str) { fInstitute = str; }
    void setFileFormat(const QString str) { fFileFormat = str; }
    void setMsrDefaultFilePath(const QString str) { fMsrDefaultFilePath = str; }

  private:
    friend class PAdminXMLParser;

    QString fExecPath;
    QString fDefaultSavePath;

    QString fBeamline;
    QString fInstitute;
    QString fFileFormat;

    QString fMsrDefaultFilePath;
};

#endif // _PADMIN_H_
