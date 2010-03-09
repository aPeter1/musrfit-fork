/****************************************************************************

  PGetDefaultDialog.cpp

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

#include <QObject>
#include <QComboBox>
#include <QMessageBox>

#include "PHelp.h"

#include "PGetDefaultDialog.h"

#define INSTITUTE_PSI    0
#define INSTITUTE_RAL    1
#define INSTITUTE_TRIUMF 2
#define INSTITUTE_JPARC  3

#define FILE_FORMAT_NEXUS    0
#define FILE_FORMAT_ROOT_NPP 1
#define FILE_FORMAT_ROOT_PPC 2
#define FILE_FORMAT_PSIBIN   3
#define FILE_FORMAT_MUD      4
#define FILE_FORMAT_WKM      5
#define FILE_FORMAT_ASCII    6
#define FILE_FORMAT_DB       7


//---------------------------------------------------------------------------
/**
 * <p>
 */
PGetDefaultDialog::PGetDefaultDialog()
{
  setupUi(this);

  setModal(true);
}


//---------------------------------------------------------------------------
/**
 * <p>
 */
void PGetDefaultDialog::setInstitute(const QString &str) {

  for (int i=0; i<fInstitute_comboBox->count(); i++) {
    if (fInstitute_comboBox->itemText(i).toLower() == str.toLower()) {
      fInstitute_comboBox->setCurrentIndex(i);
      break;
    }
  }

}

//---------------------------------------------------------------------------
/**
 * <p>
 */
void PGetDefaultDialog::setFileFormat(const QString &str)
{

  for (int i=0; i<fFileFormat_comboBox->count(); i++) {
    if (fFileFormat_comboBox->itemText(i).toLower() == str.toLower()) {
      fFileFormat_comboBox->setCurrentIndex(i);
      break;
    }
  }

}

//---------------------------------------------------------------------------
/**
 * <p>
 */
void PGetDefaultDialog::helpContent()
{
  PHelp *help = new PHelp("https://wiki.intranet.psi.ch/MUSR/MusrFit#4_5_The_RUN_Block");

  help->show();
}

//---------------------------------------------------------------------------
// END
//---------------------------------------------------------------------------
