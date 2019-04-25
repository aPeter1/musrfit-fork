/****************************************************************************

  PGetTitleBlockDialog.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

*****************************************************************************/

/***************************************************************************
 *   Copyright (C) 2010-2019 by Andreas Suter                              *
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

#include <QMessageBox>

#include "PHelp.h"
#include "PGetTitleBlockDialog.h"

//---------------------------------------------------------------------------
/**
 * <p>Constructor.
 *
 * \param helpUrl help url for the title.
 */
PGetTitleBlockDialog::PGetTitleBlockDialog(const QString helpUrl) : fHelpUrl(helpUrl)
{
  setupUi(this);

  setModal(true);
}

//---------------------------------------------------------------------------
/**
 * <p>Generates a help content window showing the description of title block.
 */
void PGetTitleBlockDialog::helpContent()
{
  if (fHelpUrl.isEmpty()) {
    QMessageBox::information(this, "**INFO**", "Will eventually show a help window");
  } else {
    #ifdef _WIN32GCC
    QMessageBox::information(this, "**INFO**", "If a newer Qt version was available, a help window would be shown!");
    #else
    PHelp *help = new PHelp(fHelpUrl);
    help->show();
    #endif // _WIN32GCC
  }
}

//---------------------------------------------------------------------------
// END
//---------------------------------------------------------------------------
