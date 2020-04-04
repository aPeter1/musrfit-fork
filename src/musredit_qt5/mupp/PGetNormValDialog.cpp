/***************************************************************************

  PGetNormValDialog.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007-2020 by Andreas Suter                              *
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

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QDoubleValidator>
#include <QPushButton>

#include "PGetNormValDialog.h"

//------------------------------------------------------------------------------
PGetNormValDialog::PGetNormValDialog(double dval, QWidget *parent, Qt::WindowFlags f) :
  QDialog(parent, f), fValue(dval)
{
  QVBoxLayout *vbox = new QVBoxLayout;
  QHBoxLayout *hbox1 = new QHBoxLayout;
  QHBoxLayout *hbox2 = new QHBoxLayout;

  vbox->addLayout(hbox1);
  vbox->addLayout(hbox2);

  QLabel *lab = new QLabel("Norm Value: ");
  QLineEdit *fEdit = new QLineEdit();
  fEdit->setValidator(new QDoubleValidator());
  fEdit->setText(QString("%1").arg(fValue));
  hbox1->addWidget(lab);
  hbox1->addWidget(fEdit);

  QPushButton *ok = new QPushButton("OK");
  QPushButton *cancel = new QPushButton("Cancel");
  hbox2->addWidget(ok);
  hbox2->addWidget(cancel);

  setModal(true);

  setLayout(vbox);

  connect( fEdit, SIGNAL(textChanged(const QString&)), this, SLOT(gotValue(const QString&)));
  connect( ok, SIGNAL(clicked()), this, SLOT(accept()));
  connect( cancel, SIGNAL(clicked()), this, SLOT(reject()));
}

//------------------------------------------------------------------------------
void PGetNormValDialog::gotValue(const QString& str)
{
  fValue = str.toDouble();
}
