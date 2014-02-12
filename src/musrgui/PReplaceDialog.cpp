/****************************************************************************

  PReplaceDialog.cpp

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

#include <qpushbutton.h>
#include <qcombobox.h>
#include <qcheckbox.h>

#include "PReplaceDialog.h"

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
PReplaceDialog::PReplaceDialog(PFindReplaceData *data, const bool selection, QWidget *parent, const char *name, bool modal, WFlags f) :
    PReplaceDialogBase(parent, name, modal, f), fData(data)
{
  // if only empty text, disable find button
  if (fData->findText == "") {
    fReplace_button->setEnabled(false);
  }

  // if there is no selection, disable that option
  if (!selection) {
    fSelectedText_checkBox->setChecked(false);
    fSelectedText_checkBox->setEnabled(false);
  }

  fFind_comboBox->setCurrentText(fData->findText);
  fReplacementText_comboBox->setCurrentText(fData->replaceText);
  fCaseSensitive_checkBox->setChecked(fData->caseSensitive);
  fWholeWordsOnly_checkBox->setChecked(fData->wholeWordsOnly);
  fFromCursor_checkBox->setChecked(fData->fromCursor);
  fFindBackwards_checkBox->setChecked(fData->findBackwards);
  fPromptOnReplace_checkBox->setChecked(fData->promptOnReplace);

  if (selection) {
    fSelectedText_checkBox->setChecked(fData->selectedText);
  }
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
PFindReplaceData* PReplaceDialog::getData()
{
  fData->findText = fFind_comboBox->currentText();
  fData->replaceText = fReplacementText_comboBox->currentText();
  fData->caseSensitive = fCaseSensitive_checkBox->isChecked();
  fData->wholeWordsOnly = fWholeWordsOnly_checkBox->isChecked();
  fData->fromCursor = fFromCursor_checkBox->isChecked();
  fData->findBackwards = fFindBackwards_checkBox->isChecked();
  if (fSelectedText_checkBox->isEnabled())
    fData->selectedText = fSelectedText_checkBox->isChecked();
  fData->promptOnReplace = fPromptOnReplace_checkBox->isChecked();

  return fData;
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PReplaceDialog::onFindTextAvailable()
{
  if (fFind_comboBox->currentText() != "")
    fReplace_button->setEnabled(true);
  else
    fReplace_button->setEnabled(false);
}

//----------------------------------------------------------------------------------------------------
// END
//----------------------------------------------------------------------------------------------------
