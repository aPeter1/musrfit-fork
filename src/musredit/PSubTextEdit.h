/****************************************************************************

  PSubTextEdit.h

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

#ifndef _PSUBTEXTEDIT_H_
#define _PSUBTEXTEDIT_H_

#include <QPlainTextEdit>
#include <QDateTime>

#include "PAdmin.h"

//------------------------------------------------------------------------------------------
/**
 * <p>Class handling a single text edit tab within musredit.
 */
class PSubTextEdit : public QPlainTextEdit
{
    Q_OBJECT

  public:
    PSubTextEdit(PAdmin *admin = 0, QWidget *parent = 0);
    virtual ~PSubTextEdit() {}

  public slots:
    void insertTitle();
    void insertParameterBlock();
    void insertTheoryFunction(QString name);
    void insertTheoryBlock();
    void insertFunctionBlock();
    void insertAsymRunBlock();
    void insertSingleHistRunBlock();
    void insertNonMusrRunBlock();
    void insertCommandBlock();
    void insertFourierBlock();
    void insertPlotBlock();
    void insertStatisticBlock();

  private:
    PAdmin *fAdmin;  ///< pointer to the administration object which holds working-, executable-paths etc.
};

#endif // _PSUBTEXTEDIT_H_
