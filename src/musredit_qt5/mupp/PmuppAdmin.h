/****************************************************************************

  PmuppAdmin.h

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

#ifndef _PMUPPADMIN_H_
#define _PMUPPADMIN_H_

#include <QString>
#include <QVector>
#include <QMap>
#include <QPixmap>
#include <QtXml>

#include "mupp.h"

class PmuppAdmin;

//---------------------------------------------------------------------------
/**
 * @brief The PmuppColor class
 */
class PmuppColor {
  public:
    PmuppColor();
    virtual ~PmuppColor() {}

    QString getName() { return fName; }
    void getRGB(int &r, int &g, int &b) { r=fRed; g=fGreen; b=fBlue; }

    void setName(const QString name) { fName = name; }
    void setRGB(const int r, const int g, const int b);

  private:
    QString fName;    
    int fRed;
    int fGreen;
    int fBlue;
};

//---------------------------------------------------------------------------
/**
 * @brief The PmuppMarker class
 */
class PmuppMarker {
  public:
    PmuppMarker() { fMarker = 20; fMarkerSize = 1.0; }
    PmuppMarker(int marker, double size) : fMarker(marker), fMarkerSize(size) {}
    virtual ~PmuppMarker() {}

    void getMarker(int &marker, double &size) { marker = fMarker; size = fMarkerSize; }
    int getMarker() { return fMarker; }
    double getMarkerSize() { return fMarkerSize; }

    void setMarker(int marker) { fMarker = marker; }
    void setMarkerSize(double size) { fMarkerSize = size; }

  private:
    int fMarker;
    double fMarkerSize;
};

//---------------------------------------------------------------------------
/**
 * PAdminXMLParser is an XML parser class used to handle the mupp startup
 * XML-file called <tt>mupp_startup.xml</tt>. This startup file contains
 * necessary informations about executable pathes, online help informations,
 * default font sizes, etc.
 */
class PmuppAdminXMLParser : public QXmlDefaultHandler
{
  public:
    PmuppAdminXMLParser(PmuppAdmin*);
    virtual ~PmuppAdminXMLParser() {}

  private:
    enum EAdminKeyWords {eEmpty, eRecentFile, eMarker, eColor};

    bool startDocument();
    bool startElement( const QString&, const QString&, const QString& ,
                       const QXmlAttributes& );
    bool endElement( const QString&, const QString&, const QString& );

    bool characters(const QString&);
    bool endDocument();

    bool warning( const QXmlParseException & exception );
    bool error( const QXmlParseException & exception );
    bool fatalError( const QXmlParseException & exception );

    EAdminKeyWords  fKeyWord;   ///< key word tag to know how to handle the content
    PmuppAdmin     *fAdmin;     ///< a pointer to the main administration class object
};

//---------------------------------------------------------------------------
/**
 * The PMuppAdmin class is handling the informations contained in the XML startup file,
 * <tt>mupp_startup.xml</tt>. This startup file contains
 * necessary informations like marker style, marker color, etc. The XML parsing is done
 * with the help of the PmuppAdminXMLParser class.
 */
class PmuppAdmin : public QObject
{
  public:
    PmuppAdmin();
    virtual ~PmuppAdmin();

    void    addRecentFile(const QString str);
    int     getNumRecentFiles() { return fRecentFile.size(); }
    QString getRecentFile(int idx);

    int getNoOfMarkers() { return fMarker.size(); }
    QVector<PmuppMarker> getMarkers() { return fMarker; }
    PmuppMarker getMarker(int idx);

    int getNoOfColors() { return fColor.size(); }
    QVector<PmuppColor> getColors() { return fColor; }
    void getColor(QString name, int &r, int &g, int &b);
    void getColor(int idx, int &r, int &g, int &b);

    void setMarker(int marker, double size);
    void setColor(int r, int g, int b, QString name="");

  private:
    friend class PmuppAdminXMLParser;

    QVector<QString> fRecentFile; ///< keep vector of recent path-file names

    QVector<PmuppMarker> fMarker;
    QVector<PmuppColor> fColor;

    void saveRecentFiles(); ///< save recent file list
    void createMuppStartupFile(); ///< create default mupp_startup.xml
};

#endif // _PMUPPADMIN_H_
