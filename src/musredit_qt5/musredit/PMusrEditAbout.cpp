/****************************************************************************

  PMusrEditAbout.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

*****************************************************************************/

/***************************************************************************
 *   Copyright (C) 2010-2021 by Andreas Suter                              *
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
#include "git-revision.h"
#include "PMusrEditAbout.h"

//---------------------------------------------------------------------------
/**
 * <p>Handles the musredit about popup.
 */
PMusrEditAbout::PMusrEditAbout(QWidget *parent) : QDialog(parent)
{
  setupUi(this);

  fGitBranch_label->setText(QString("git-branch: %1").arg(GIT_BRANCH));
  fGitRev_label->setText(QString("git-rev: %1").arg(GIT_CURRENT_SHA1));
  fMusrfitVersion_label->setText(QString("musrfit-version: %1").arg(PACKAGE_VERSION));
  fRootVersion_label->setText(QString("ROOT-version: %1").arg(ROOT_VERSION_USED));

  setModal(true);
}

//---------------------------------------------------------------------------
// END
//---------------------------------------------------------------------------
