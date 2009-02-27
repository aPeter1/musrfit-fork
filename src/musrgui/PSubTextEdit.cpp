/****************************************************************************

  PSubTextEdit.cpp

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

#include <qaction.h>
#include <qpopupmenu.h>
#include <qdatetime.h>
#include <qlineedit.h>

#include "PSubTextEdit.h"
#include "forms/PGetTitleDialog.h"

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
PSubTextEdit::PSubTextEdit(QWidget *parent, const char *name) : QTextEdit(parent, name)
{
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
QPopupMenu* PSubTextEdit::createPopupMenu(const QPoint &pos)
{
  QPopupMenu *menu = new QPopupMenu( this );

  QAction *a;
  a = new QAction(tr("insert Title"), 0, this, "insertTitle");
  connect(a, SIGNAL( activated() ), this, SLOT( insertTitle() ));
  a->addTo( menu );

  menu->insertSeparator();

  a = new QAction(tr("insert Parameter Block"), 0, this, "insertParameterBlock");
  connect(a, SIGNAL( activated() ), this, SLOT( insertParameterBlock() ));
  a->addTo( menu );

  a = new QAction(tr("insert Parameter"), 0, this, "insertParameter");
  connect(a, SIGNAL( activated() ), this, SLOT( insertParameter() ));
  a->addTo( menu );

  menu->insertSeparator();

  a = new QAction(tr("insert Theory Block"), 0, this, "insertTheoryBlock");
  connect(a, SIGNAL( activated() ), this, SLOT( insertTheoryBlock() ));
  a->addTo( menu );

  a = new QAction(tr("insert Function Block"), 0, this, "insertFunctionBlock");
  connect(a, SIGNAL( activated() ), this, SLOT( insertFunctionBlock() ));
  a->addTo( menu );

  a = new QAction(tr("insert Asymmetry Block"), 0, this, "insertAsymRunBlock");
  connect(a, SIGNAL( activated() ), this, SLOT( insertAsymRunBlock() ));
  a->addTo( menu );

  a = new QAction(tr("insert Single Histo Block"), 0, this, "insertSingleHistRunBlock");
  connect(a, SIGNAL( activated() ), this, SLOT( insertSingleHistRunBlock() ));
  a->addTo( menu );

  a = new QAction(tr("insert NonMusr Block"), 0, this, "insertNonMusrRunBlock");
  connect(a, SIGNAL( activated() ), this, SLOT( insertNonMusrRunBlock() ));
  a->addTo( menu );

  a = new QAction(tr("insert Command Block"), 0, this, "insertCommandBlock");
  connect(a, SIGNAL( activated() ), this, SLOT( insertCommandBlock() ));
  a->addTo( menu );

  a = new QAction(tr("insert Fourier Block"), 0, this, "insertFourierBlock");
  connect(a, SIGNAL( activated() ), this, SLOT( insertFourierBlock() ));
  a->addTo( menu );

  a = new QAction(tr("insert Plot Block"), 0, this, "insertPlotBlock");
  connect(a, SIGNAL( activated() ), this, SLOT( insertPlotBlock() ));
  a->addTo( menu );

  a = new QAction(tr("insert Statistic Block"), 0, this, "insertStatisticBlock");
  connect(a, SIGNAL( activated() ), this, SLOT( insertStatisticBlock() ));
  a->addTo( menu );

  return menu;
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PSubTextEdit::insertTitle()
{
  PGetTitleDialog *dlg = new PGetTitleDialog();
  if (dlg->exec() == QDialog::Accepted) {
    QString title = dlg->fTitle_lineEdit->text();
    insert(title+"\n");
    insert("###############################################################\n");
  }
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PSubTextEdit::insertParameterBlock()
{
  insert("will insert a real parameter block");
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PSubTextEdit::insertParameter()
{
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PSubTextEdit::insertTheoryBlock()
{
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PSubTextEdit::insertFunctionBlock()
{
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PSubTextEdit::insertAsymRunBlock()
{
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PSubTextEdit::insertSingleHistRunBlock()
{
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PSubTextEdit::insertNonMusrRunBlock()
{
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PSubTextEdit::insertCommandBlock()
{
  insert("###############################################################\n");
  insert("COMMANDS\n");
  insert("SET BATCH\n");
  insert("STRATEGY 1\n");
  insert("MINIMIZE\n");
  insert("#MINOS\n");
  insert("SAVE\n");
  insert("END RETURN\n\n");
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PSubTextEdit::insertFourierBlock()
{
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PSubTextEdit::insertPlotBlock()
{
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PSubTextEdit::insertStatisticBlock()
{
  QDateTime dt = QDateTime::currentDateTime();
  insert("###############################################################\n");
  insert("STATISTIC --- " + dt.toString("yyyy-MM-dd hh:mm:ss") + "\n");
  insert("chisq = ????, NDF = ????, chisq/NDF = ????\n\n");
}

//----------------------------------------------------------------------------------------------------
// END
//----------------------------------------------------------------------------------------------------
