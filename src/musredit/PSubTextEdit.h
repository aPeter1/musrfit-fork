/****************************************************************************

  PSubTextEdit.h

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

  $Id: PSubTextEdit.h 3936 2009-05-22 11:38:21Z nemu $

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

class PSubTextEdit : public QPlainTextEdit
{
    Q_OBJECT

  public:
    PSubTextEdit(PAdmin *admin = 0, QWidget *parent = 0);
    virtual ~PSubTextEdit() {}

    void setLastModified(const QDateTime &lastModified) { fLastModified = lastModified; }
    QDateTime getLastModified() const { return fLastModified; }

  public slots:
    void insertTitle();
    void insertParameterBlock();
    void insertTheoryFunction(int idx);
    void insertTheoryBlock();
    void insertFunctionBlock();
    void insertAsymRunBlock();
    void insertSingleHistRunBlock();
    void insertNonMusrRunBlock();
    void insertCommandBlock();
    void insertFourierBlock();
    void insertPlotBlock();
    void insertStatisticBlock();

  protected:
    virtual QMenu *createPopupMenu( const QPoint &pos);

  private:
    PAdmin *fAdmin;
    QDateTime fLastModified;
};

#endif // _PSUBTEXTEDIT_H_
