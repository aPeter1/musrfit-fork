/****************************************************************************

  PFindDialog.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

*****************************************************************************/

/***************************************************************************
 *   Copyright (C) 2009-2016 by Andreas Suter                              *
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

#include <QPushButton>
#include <QComboBox>
#include <QCheckBox>

#include <QMessageBox>

#include "PFindDialog.h"

//----------------------------------------------------------------------------------------------------
/**
 * <p>Sets up the find dialog.
 *
 * \param data pointer to the find/replace data structure needed to perform the task.
 * \param selection flag indicating if the find shall be restricted to the selected area
 * \param parent pointer to the parent object
 * \param f qt specific window flags
 */
PFindDialog::PFindDialog(PFindReplaceData *data, const bool selection, QWidget *parent, Qt::WindowFlags f) :
    QDialog(parent, f), fData(data)
{
  setupUi(this);

  setModal(true);

  // if only empty text, disable find button
  if (fData->findText == "") {
    fFind_pushButton->setEnabled(false);
  }

  // if there is no selection, disable that option
  if (!selection) {
    fSelectedText_checkBox->setChecked(false);
    fSelectedText_checkBox->setEnabled(false);
  }

  fFind_comboBox->setItemText(0, fData->findText);
  fCaseSensitive_checkBox->setChecked(fData->caseSensitive);
  fWholeWordsOnly_checkBox->setChecked(fData->wholeWordsOnly);
  fFromCursor_checkBox->setChecked(fData->fromCursor);
  fFindBackwards_checkBox->setChecked(fData->findBackwards);

  if (selection) {
    fSelectedText_checkBox->setChecked(fData->selectedText);
  }
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Extracts all the necessary informations from the find dialog, feeds it to the find/replace
 * structure and returns a point to this structure.
 */
PFindReplaceData* PFindDialog::getData()
{
  fData->findText = fFind_comboBox->currentText();
  fData->caseSensitive = fCaseSensitive_checkBox->isChecked();
  fData->wholeWordsOnly = fWholeWordsOnly_checkBox->isChecked();
  fData->fromCursor = fFromCursor_checkBox->isChecked();
  fData->findBackwards = fFindBackwards_checkBox->isChecked();
  if (fSelectedText_checkBox->isEnabled())
    fData->selectedText = fSelectedText_checkBox->isChecked();

  return fData;
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Enables the find button only if there is any find text entered.
 */
void PFindDialog::onFindTextAvailable(const QString&)
{
  if (fFind_comboBox->currentText() != "")
    fFind_pushButton->setEnabled(true);
  else
    fFind_pushButton->setEnabled(false);
}

//----------------------------------------------------------------------------------------------------
// END
//----------------------------------------------------------------------------------------------------
