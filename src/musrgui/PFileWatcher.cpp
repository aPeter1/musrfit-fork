/****************************************************************************

  PFileWatcher.cpp

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

#include <qfileinfo.h>
#include <qstring.h>
#include <qdatetime.h>
#include <qtimer.h>

#include <qmessagebox.h>

#include "PFileWatcher.h"

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
PFileWatcher::PFileWatcher(const QString &fileName, const QDateTime &lastModified) : fFileName(fileName), fLastModified(lastModified)
{
  fFileInfo = 0;
  fFileInfo = new QFileInfo(fFileName);
  if (!fFileInfo) {
    fValid = false;
  }
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
PFileWatcher::~PFileWatcher()
{
  if (fFileInfo) {
    delete fFileInfo;
    fFileInfo = 0;
  }
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
bool PFileWatcher::modified()
{
  bool result = false;

  fFileInfo->refresh();

  if (fFileInfo->lastModified() > fLastModified) {
    fLastModified = fFileInfo->lastModified();
    result = true;
  }

  return result;
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PFileWatcher::modified(int timeout)
{
  fTimerCheck = new QTimer(this);

  connect( fTimerCheck, SIGNAL(timeout()), this, SLOT(checkIfModified()) );
  QTimer::singleShot(timeout * 1000, this, SLOT(stopFileCheck()));

  fTimerCheck->start(1000);
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PFileWatcher::checkIfModified()
{
  fFileInfo->refresh();

  if (fFileInfo->lastModified() > fLastModified) {
    fLastModified = fFileInfo->lastModified();
    fTimerCheck->stop();
    emit changed();
  }
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PFileWatcher::stopFileCheck()
{
  fTimerCheck->stop();
}
