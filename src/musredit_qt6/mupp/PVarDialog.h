/***************************************************************************

  PVarDialog.h

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007-2023 by Andreas Suter                              *
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

#ifndef _PVARDIALOG_H_
#define _PVARDIALOG_H_

#include <QDialog>
#include <QPushButton>
#include <QPlainTextEdit>
#include <QListWidget>
#include <QVector>

//-----------------------------------------------------------------------------
/**
 * @brief The PCollInfo struct
 */
struct PCollInfo
{
  QString fCollName; ///< collection name
  QStringList fVarName; ///< variable names of the given collection
};

//-----------------------------------------------------------------------------
/**
 * @brief The PShowVarNameDialog class. Class used to create a show variable
 * name dialog.
 */
class PShowVarNameDialog : public QDialog
{
  Q_OBJECT

  public:
    PShowVarNameDialog(PCollInfo &info);
};

//-----------------------------------------------------------------------------
/**
 * @brief The PVarDialog class
 */
class PVarDialog : public QDialog
{
  Q_OBJECT

  public:
    PVarDialog(QVector<PCollInfo> collection_list, bool darkTheme,
               QWidget *parent = nullptr,
               Qt::WindowFlags f = Qt::WindowFlags());

  private:
    QPlainTextEdit *fVarEdit;
    QListWidget *fCollectionView;
    QPushButton *fCancel;
    QPushButton *fAdd;
    QPushButton *fCheck;
    QPushButton *fHelp;
    QPushButton *fShowVarName;

    QVector<PCollInfo> fCollList;

    bool basic_check();
    bool var_consistency_check();
    QStringList collectVarNames(QStringList &list, bool& ok);
    bool hasErrorDef(QStringList &varNames, QString& name);

  private slots:
    void check();
    void add();
    void help();
    void showVarNames();

  signals:
    void check_request(QString varStr, QVector<int> idx);
    void add_request(QString varStr, QVector<int> idx);
};

#endif // _PVARDIALOG_H_
