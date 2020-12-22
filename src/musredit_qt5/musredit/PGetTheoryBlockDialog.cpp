/****************************************************************************

  PGetTheoryBlockDialog.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

*****************************************************************************/

/***************************************************************************
 *   Copyright (C) 2009-2019 by Andreas Suter                              *
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
#include <QDesktopServices>
#include <QUrl>

#include <QtDebug>

#include "PGetTheoryBlockDialog.h"

//----------------------------------------------------------------------------------------------------
/**
 * <p>Constructor.
 *
 * \param admin pointer to the administration class object needed to extract the default theory set informations.
 * \param helpUrl help url for the asymmetry run block
 */
PGetTheoryBlockDialog::PGetTheoryBlockDialog(PAdmin *admin, const QString helpUrl) :
              fAdmin(admin),
              fHelpUrl(helpUrl)
{
  setupUi(this);

  setModal(true);

  // feed theory function combo box
  fTheoryFunction_comboBox->setIconSize(QSize(250, 20));
  PTheory *theoItem;
  QIcon *icon;
  QString iconName;
  for (unsigned int i=0; i<fAdmin->getTheoryCounts();  i++) {
    theoItem = fAdmin->getTheoryItem(i);
    if (theoItem->pixmapName.length() > 0) {
      iconName = QString(":/latex_images/") + theoItem->pixmapName;      
      icon = new QIcon(QPixmap(iconName));
      fTheoryFunction_comboBox->insertItem(i, *icon, theoItem->label);
    } else {
      fTheoryFunction_comboBox->insertItem(i, theoItem->label);
    }
  }
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>returns the theory function string of the currently selected theory function.
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
 * <p>adds the current theory function to the text field and additionally adds a '+'.
 */
void PGetTheoryBlockDialog::addPlus()
{
  QString str = getTheoFuncString() + "\n+";
  fTheoryBlock_plainTextEdit->appendPlainText(str);
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>adds the current theory function to the text field (newline == '*').
 */
void PGetTheoryBlockDialog::addMultiply()
{
  QString str = getTheoFuncString();
  fTheoryBlock_plainTextEdit->appendPlainText(str);
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Generates a help content window showing the description of the theory block.
 */
void PGetTheoryBlockDialog::helpContent()
{
  if (fHelpUrl.isEmpty()) {
    QMessageBox::information(this, "**INFO**", "Will eventually show a help window");
  } else {
    bool ok = QDesktopServices::openUrl(QUrl(fHelpUrl, QUrl::TolerantMode));
    if (!ok) {
      QString msg = QString("<p>Sorry: Couldn't open default web-browser for the help.<br>Please try: <a href=\"%1\">musrfit docu</a> in your web-browser.").arg(fHelpUrl);
      QMessageBox::critical( nullptr,
                             tr("Fatal Error"),
                             msg,
                             tr("Quit") );
    }
  }
}

//----------------------------------------------------------------------------------------------------
// END
//----------------------------------------------------------------------------------------------------
