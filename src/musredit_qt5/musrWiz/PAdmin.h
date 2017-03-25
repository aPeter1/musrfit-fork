/****************************************************************************

  PAdmin.h

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

*****************************************************************************/

/***************************************************************************
 *   Copyright (C) 2010-2016 by Andreas Suter                              *
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

#include <QObject>
#include <QVector>
#include <QtXml>

#include "PTheoTemplate.h"
#include "PMusrfitFunc.h"
#include "PInstrumentDef.h"

class PAdmin;

//---------------------------------------------------------------------------
class PFuncXMLParser : public QXmlDefaultHandler
{
  public:
    PFuncXMLParser(PAdmin*);
    virtual ~PFuncXMLParser() {}

  private:
    enum EFuncKeyWords {eEmpty,
                        eTemplateName, eTemplateTheo, eTemplateFunc,
                        eName, eAbbrv, eNoOfParam, eParam,
                        eParamName, eParamValue, eParamMap};

    bool startDocument();
    bool startElement(const QString&, const QString&, const QString& ,
                      const QXmlAttributes& qAttr);
    bool endElement( const QString&, const QString&, const QString& );

    bool characters(const QString&);
    bool endDocument();

    bool warning( const QXmlParseException & exception );
    bool error( const QXmlParseException & exception );
    bool fatalError( const QXmlParseException & exception );

    EFuncKeyWords fKeyWord;    ///< key word tag to know how to handle the content
    PAdmin        *fAdmin;     ///< a pointer to the main administration class object

    PTheoTemplate fTheoTemplate;
    PMusrfitFunc fFunc;
    PFuncParam fParam;
};

//---------------------------------------------------------------------------
class PInstrumentDefXMLParser : public QXmlDefaultHandler
{
  public:
    PInstrumentDefXMLParser(PAdmin*);
    virtual ~PInstrumentDefXMLParser() {}

  private:
    enum EKeyWords {eEmpty, eInstitute, eInstrument, eRunNameTemplate,
                    eBeamline, eDataFileFormat, eTf, eZf, eLf,
                    eNoOfDetectors, eFgbOffset, eLgb, eBkgRange,
                    eLogicDetector};

    bool startDocument();
    bool startElement(const QString&, const QString&, const QString& ,
                      const QXmlAttributes& qAttr);
    bool endElement( const QString&, const QString&, const QString& );

    bool characters(const QString&);
    bool endDocument();

    bool warning( const QXmlParseException & exception );
    bool error( const QXmlParseException & exception );
    bool fatalError( const QXmlParseException & exception );

    EKeyWords fKeyWord;    ///< key word tag to know how to handle the content
    PAdmin    *fAdmin;     ///< a pointer to the main administration class object

    QString fInstituteName;
    PInstrument *fInstrument;
    PSetup *fSetup;
};

//---------------------------------------------------------------------------
class PMusrWizDefault
{
  public:
    PMusrWizDefault();
    ~PMusrWizDefault() {}

    QString getInstitute() { return fInstitute; }
    QString getInstrument() { return fInstrument; }
    QString getFitType() { return fFitType; }

    void setInstitute(QString str) { fInstitute = str; }
    void setInstrument(QString str) { fInstrument = str; }
    void setFitType(QString str) { fFitType = str; }

  private:
    QString fInstitute;
    QString fInstrument;
    QString fFitType;
};

//---------------------------------------------------------------------------
class PMusrWizDefaultXMLParser : public QXmlDefaultHandler
{
  public:
    PMusrWizDefaultXMLParser(PAdmin*);
    virtual ~PMusrWizDefaultXMLParser() {}

  private:
    enum EKeyWords {eEmpty, eInstitute, eInstrument, eFitType};

    bool startDocument();
    bool startElement(const QString&, const QString&, const QString& ,
                      const QXmlAttributes& );
    bool endElement( const QString&, const QString&, const QString& );

    bool characters(const QString&);
    bool endDocument();

    bool warning( const QXmlParseException & exception );
    bool error( const QXmlParseException & exception );
    bool fatalError( const QXmlParseException & exception );

    EKeyWords fKeyWord;    ///< key word tag to know how to handle the content
    PAdmin    *fAdmin;     ///< a pointer to the main administration class object

    PMusrWizDefault fDefault;
};

//---------------------------------------------------------------------------
class PAdmin : public QObject
{
  public:
    PAdmin();
    ~PAdmin() {}

    bool IsValid() { return fValid; }
    void dump(int tag);

    QString getDefaultInstitute() { return fDefault.getInstitute(); }
    QString getDefaultInstrument() { return fDefault.getInstrument(); }
    QString getDefaultFitType() { return fDefault.getFitType(); }

    QStringList getInstituteList();
    QStringList getInstrumentList(QString institute);
    PInstrument *getInstrument(QString institute, QString instrument);
    int getTheoTemplateSize() { return fTheoTemplate.size(); }
    QVector<PTheoTemplate> getTheoTemplates() { return fTheoTemplate; }
    PTheoTemplate getTheoTemplate(int idx);
    int getMusrfitFuncSize() { return fMusrfitFunc.size(); }
    QVector<PMusrfitFunc> getMusrfitFunc() { return fMusrfitFunc; }
    PMusrfitFunc getMusrfitFunc(QString name);

  protected:
    int addInstrument(QString institute, PInstrument instrument);

  private:
    friend class PFuncXMLParser;
    friend class PInstrumentDefXMLParser;
    friend class PMusrWizDefaultXMLParser;

    bool fValid;

    PMusrWizDefault fDefault;
    QVector<PInstitute> fInstitute;
    QVector<PTheoTemplate> fTheoTemplate;
    QVector<PMusrfitFunc> fMusrfitFunc;

    int loadMusrWizDefault(QString fln);
    int loadMusrfitFunc(QString fln);
    int loadInstrumentDef(QString fln);
};

#endif // _PADMIN_H_
