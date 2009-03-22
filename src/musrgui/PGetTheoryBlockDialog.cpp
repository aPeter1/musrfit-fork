/****************************************************************************

  PGetTheoryBlockDialog.cpp

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

#include <qmessagebox.h>
#include <qtextedit.h>
#include <qcombobox.h>
#include <qpixmap.h>
#include <qimage.h>

#include "PGetTheoryBlockDialog.h"

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
PGetTheoryBlockDialog::PGetTheoryBlockDialog(PAdmin *admin,
                                             QWidget * parent, const char *name,
                                             bool modal, WFlags f) :
              PGetTheoryBlockDialogBase(parent, name, modal, f),
              fAdmin(admin)
{
  // feed theory function combo box
  QString pixmapPath = fAdmin->getTheoFuncPixmapPath();
  PTheory *theoItem;
  for (unsigned int i=0; i<fAdmin->getTheoryCounts();  i++) {
    theoItem = fAdmin->getTheoryItem(i);
    if (theoItem->pixmapName.length() > 0) {
      QPixmap pixmap( QImage(pixmapPath+"/"+theoItem->pixmapName, "PNG") );
      fTheoryFunction_comboBox->insertItem(pixmap, theoItem->label);
    } else {
      fTheoryFunction_comboBox->insertItem(theoItem->label);
    }
  }
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PGetTheoryBlockDialog::helpContents()
{
  QMessageBox::information(this, "helpContents",
                           fAdmin->getHelpMain(), QMessageBox::Ok);
}

//----------------------------------------------------------------------------------------------------
// END
//----------------------------------------------------------------------------------------------------
