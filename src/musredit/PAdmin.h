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

#include <musredit.h>

class PAdmin;

//---------------------------------------------------------------------------
/**
 * <p>This structure is keeping informations necessary to handle musrfit
 * theory functions (see also <code>https://intranet.psi.ch/MUSR/MusrFit#4_3_The_THEORY_Block</code>).
 */
typedef struct {
  QString name;
  QString comment;
  QString label;
  QString pixmapName;
  QPixmap pixmap;
  int     params;
} PTheory;

//---------------------------------------------------------------------------
/**
 * PAdminXMLParser is an XML parser class used to handle the musredit startup
 * XML-file called <tt>musredit_startup.xml</tt>. This startup file contains
 * necessary informations about executable pathes, online help informations,
 * default font sizes, etc.
 */
class PAdminXMLParser : public QXmlDefaultHandler
{
  public:
    PAdminXMLParser(PAdmin*);
    virtual ~PAdminXMLParser() {}

  private:
    enum EAdminKeyWords {eEmpty, eTimeout, eFontName, eFontSize, eExecPath, eDefaultSavePath, eTitleFromDataFile, eEnableMusrT0,
                         eBeamline, eInstitute, eFileFormat, eLifetimeCorrection, eMsrDefaultFilePath,
                         eTheoFuncPixmapPath, eFunc, eFuncName, eFuncComment, eFuncLabel,
                         eFuncPixmap, eFuncParams, eHelpMain, eHelpTitle, eHelpParameters, eHelpTheory, eHelpFunctions,
                         eHelpRun, eHelpCommand, eHelpFourier, eHelpPlot, eHelpStatistic, eHelpMsr2Data,
                         eChainFit, eWriteDataHeader, eIgnoreDataHeaderInfo, eKeepMinuit2Output, eWriteColumnData,
                         eRecreateDataFile, eOpenFileAfterFitting, eCreateMsrFileOnly, eFitOnly, eGlobal, eGlobalPlus};

    bool startDocument();
    bool startElement( const QString&, const QString&, const QString& ,
                       const QXmlAttributes& );
    bool endElement( const QString&, const QString&, const QString& );

    bool characters(const QString&);
    bool endDocument();

    bool warning( const QXmlParseException & exception );
    bool error( const QXmlParseException & exception );
    bool fatalError( const QXmlParseException & exception );

    QString expandPath(const QString&);

    EAdminKeyWords fKeyWord;    ///< key word tag to know how to handle the content
    bool           fFunc;       ///< flag needed to indicate that a new theory function is found
    PTheory        fTheoryItem; ///< holding the informations necessary for a theory item
    PAdmin         *fAdmin;     ///< a pointer to the main administration class object
};

//---------------------------------------------------------------------------
/**
 * The PAdmin class is handling the informations contained in the XML startup file,
 * <tt>musredit_startup.xml</tt>. This startup file contains
 * necessary informations about executable pathes, online help informations,
 * default font sizes, etc. The XML parsing is done with the help of the PAdminXMLParser
 * class.
 */
class PAdmin
{
  public:
    PAdmin();
    virtual ~PAdmin() {}

    int     getTimeout() { return fTimeout; }
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
    PMsr2DataParam getMsr2DataParam() { return fMsr2DataParam; }

    void setFontName(const QString str) { fFontName = str; }
    void setFontSize(const int ival) { fFontSize = ival; }

  protected:
    void setTimeout(const int ival) { fTimeout = ival; }
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

    int fTimeout;       ///< timeout in seconds

    QString fFontName;  ///< default font name
    int fFontSize;      ///< default font size

    QString fExecPath;           ///< system path to the musrfit executables
    QString fDefaultSavePath;    ///< default path where the msr-file should be saved
    QString fMsrDefaultFilePath; ///< path where to find musredit source
    QString fTheoFuncPixmapPath; ///< path where the default pixmaps can be found

    bool fTitleFromDataFile; ///< flag indicating if the title should be extracted from the data file (default settings).
    bool fEnableMusrT0;      ///< flag indicating if musrT0 shall be enabled at startup from within musredit (default settings)

    QString fBeamline;   ///< name of the beamline. Used to generate default run header lines.
    QString fInstitute;  ///< name of the institute. Used to generate default run header lines.
    QString fFileFormat; ///< default file format. Used to generate default run header lines.
    bool fLifetimeCorrection; ///< flag indicating if by default the lifetime-correction-flag in a single histo file shall be set.

    mutable PMsr2DataParam fMsr2DataParam; ///< keeps msr2data default parameter flags

    QMap<QString, QString> fHelpUrl; ///< maps tag to help url

    QVector<PTheory> fTheory; ///< stores all known theories. Needed when generating theory blocks from within musredit.
};

#endif // _PADMIN_H_
