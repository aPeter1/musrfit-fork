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

#include <QMessageBox>
#include <QTextEdit>
#include <QComboBox>
#include <QPixmap>
#include <QImage>

#include <QtDebug>

#include "PGetTheoryBlockDialog.h"

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
PGetTheoryBlockDialog::PGetTheoryBlockDialog(PAdmin *admin, QWidget * parent, Qt::WindowFlags f) :
              QDialog(parent, f),
              fAdmin(admin)
{
  setupUi(this);

  setModal(true);

  // feed theory function combo box
  PTheory *theoItem;
  QIcon icon;
  QString iconName;
  for (unsigned int i=0; i<fAdmin->getTheoryCounts();  i++) {
    theoItem = fAdmin->getTheoryItem(i);
    if (theoItem->pixmapName.length() > 0) {
      iconName = QString(":/latex_images/") + theoItem->pixmapName;
      icon.addPixmap(QPixmap(iconName));
      fTheoryFunction_comboBox->insertItem(i, icon, theoItem->label);
    } else {
      fTheoryFunction_comboBox->insertItem(i, theoItem->label);
    }
  }
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
QString PGetTheoryBlockDialog::getTheoFuncString()
{
  QString str = "????";
  int idx = fTheoryFunction_comboBox->currentIndex();
  PTheory *theoItem = fAdmin->getTheoryItem(idx);
  if (theoItem == 0)
    return str;

  // add theory function name
  str = theoItem->name + "   ";
  if (theoItem->name == "userFcn") {
    str += "libMyLibrary.so  TMyFunction  ";
  }
  // add pseudo parameters
  for (int i=0; i<theoItem->params; i++) {
    str += QString("%1").arg(i+1) + "   ";
  }
  // add comment
  str += theoItem->comment;

  return str;
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PGetTheoryBlockDialog::addPlus()
{
  QString str = getTheoFuncString() + "\n+";
  fTheoryBlock_plainTextEdit->appendPlainText(str);
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PGetTheoryBlockDialog::addMultiply()
{
  QString str = getTheoFuncString();
  fTheoryBlock_plainTextEdit->appendPlainText(str);
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PGetTheoryBlockDialog::helpContent()
{
  QMessageBox::information(this, "helpContents",
                           fAdmin->getHelpMain(), QMessageBox::Ok);
}

//----------------------------------------------------------------------------------------------------
// END
//----------------------------------------------------------------------------------------------------
