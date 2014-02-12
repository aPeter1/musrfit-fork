/****************************************************************************

  PFileWatcher.h

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

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

#ifndef _PFILEWATCHER_H_
#define _PFILEWATCHER_H_

#include <qobject.h>
#include <qfileinfo.h>

class PFileWatcher : public QObject
{
  Q_OBJECT

  public:
    PFileWatcher(const QString &fileName, const QDateTime &lastModified);
    virtual ~PFileWatcher();

    virtual bool isValid() { return fValid; }
    virtual bool modified();
    virtual void modified(int timeout);

  signals:
    void changed();

  private slots:
    void checkIfModified();
    void stopFileCheck();

  private:
    bool       fValid;
    QString    fFileName;
    QFileInfo *fFileInfo;
    QDateTime  fLastModified;

    QTimer    *fTimerCheck;
};

#endif // _PFILEWATCHER_H_
