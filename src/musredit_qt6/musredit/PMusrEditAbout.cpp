/****************************************************************************

  PMusrEditAbout.cpp

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

#include "config.h"
#ifdef HAVE_GIT_REV_H
#include "git-revision.h"
#endif
#include "PMusrEditAbout.h"

//---------------------------------------------------------------------------
/**
 * <p>Handles the musredit about popup.
 */
PMusrEditAbout::PMusrEditAbout(QWidget *parent) : QDialog(parent)
{
  setupUi(this);

#ifdef HAVE_GIT_REV_H
  fGitBranch_label->setText(QString("git-branch: %1").arg(GIT_BRANCH));
  fGitRev_label->setText(QString("git-rev: %1").arg(GIT_CURRENT_SHA1));
#else
  fGitBranch_label->setText(QString("git-branch: unknown"));
  fGitRev_label->setText(QString("git-rev: unknown"));
#endif
  fMusrfitVersion_label->setText(QString("musrfit-version: %1 (%2)").arg(PACKAGE_VERSION).arg(BUILD_TYPE));
  fRootVersion_label->setText(QString("ROOT-version: %1").arg(ROOT_VERSION_USED));

  setModal(true);
}

//---------------------------------------------------------------------------
// END
//---------------------------------------------------------------------------
