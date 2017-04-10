/****************************************************************************

  PHelp.h

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

#ifndef _PHELP_H_
#define _PHELP_H_

#include <QtWidgets>

class QWebEngineView;
QT_BEGIN_NAMESPACE
class QLineEdit;
QT_END_NAMESPACE

//-------------------------------------------------------------------------
/**
 * <p>Class providing the help interface for the user. It opens the wiki docu
 * pages such that a user has the chance to read.
 */
class PHelp : public QMainWindow
{
  Q_OBJECT

  public:
    PHelp(const QString &url, const bool isDarkTheme=false);
    virtual ~PHelp();

  protected slots:
    void done();
    void adjustLocation();
    void changeLocation();
    void adjustTitle();
    void setProgress(int p);
    void finishLoading(bool);

  private:
    bool fDarkTheme;
    QWebEngineView *fView; ///< web viewer
    QLineEdit *fLocationEdit; ///< url address line edit
    int fProgress; ///< progress value (0-100) while loading an url
};

#endif // _PHELP_H_
