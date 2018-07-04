/***************************************************************************

  PMusrStep.h

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007-2016 by Andreas Suter                              *
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

#ifndef _PMUSRSTEP_H_
#define _PMUSRSTEP_H_

#include <QDialog>
#include <QWidget>
#include <QVector>
#include <QPushButton>
#include <QTableWidget>
#include <QLabel>
#include <QCheckBox>

typedef struct {
  QString number;
  QString name;
  QString value;
  QString step;
  QString posErr;
  QString boundLow;
  QString boundHigh;
} PParam;

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class PModSelect : public QDialog
{
  Q_OBJECT

  public:
    PModSelect(QWidget *parent=Q_NULLPTR);

  signals:
    void scale(bool automatic, double factor, bool absVal);

  private slots:
    void absoluteValueStateChanged(int);
    void scaleAuto();
    void getFactor();

  private:
    QCheckBox *fAbsVal;
    QLabel *fFactorLabel;
    QLineEdit *fFactorLineEdit;
    QPushButton *fScaleByFactor;
    QPushButton *fScaleAutomatic;
    QPushButton *fCancel;
};

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class PMusrStep : public QDialog
{
  Q_OBJECT

  public:
    PMusrStep(const char *fln, QWidget *parent=Q_NULLPTR);
    bool isValid() { return fValid; }

  private slots:
    void handleCellChanged(int row, int column);
    void checkSpecific();
    void checkAll();
    void unCheckAll();
    void modifyAuto();
    void modifyChecked();
    void saveAndQuit();
    void handleModSelect(bool automatic, double factor, bool absVal);

  private:
    QString fMsrFileName;
    bool fValid;

    QVector<PParam> fParamVec;

    QLabel       *fTitleLabel;
    QTableWidget *fParamTable;
    QPushButton  *fCheckSpecific;
    QPushButton  *fCheckAll;
    QPushButton  *fUnCheckAll;
    QPushButton  *fModifyAuto;
    QPushButton  *fModifySelected;
    QPushButton  *fSave;
    QPushButton  *fCancel;

    PModSelect *fModSelect;

    void initParam(PParam &param);
    double lookupTable(const QString str, bool &absVal);
    QString adoptStep(const QString str, double factor, bool absVal);
    int readMsrFile();
    int writeMsrFile();
    QString getLine(QByteArray &data, int &idx);
    QString updateParamLine(const QString str);
};

#endif // _PMUSRSTEP_H_
