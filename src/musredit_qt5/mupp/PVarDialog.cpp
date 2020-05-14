/***************************************************************************

  PVarDialog.cpp

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

#include <iostream>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QLabel>
#include <QRegularExpression>

#include "PVarDialog.h"

//--------------------------------------------------------------------------
/**
 * @brief PShowVarNameDialog::PShowVarNameDialog. Ctor
 * @param info containing the necessary information of the collection
 */
PShowVarNameDialog::PShowVarNameDialog(PCollInfo &info)
{
  // if fCollName is a path name, extract the fln
  QString collNameStr(info.fCollName);
  if (collNameStr.contains("/")) {
    QStringList tok = collNameStr.split('/', QString::SkipEmptyParts);
    collNameStr = tok[tok.count()-1];
  }
  QLabel *collName = new QLabel(collNameStr);
  QLabel *numVars = new QLabel(QString("#variables: %1").arg(info.fVarName.count()));
  QListWidget *list = new QListWidget();
  for (int i=0; i<info.fVarName.count(); i++) {
    QListWidgetItem *newItem = new QListWidgetItem;
    newItem->setText(QString("%1 : %2").arg(i, 2).arg(info.fVarName[i]));
    list->addItem(newItem);
  }
  QPushButton *done = new QPushButton("Done", this);

  QVBoxLayout *vLayout = new QVBoxLayout;
  vLayout->addWidget(collName);
  vLayout->addWidget(numVars);
  vLayout->addWidget(list);
  vLayout->addWidget(done);

  connect(done, SIGNAL( clicked() ), this, SLOT( accept() ));

  resize(300, 450);
  setLayout(vLayout);
  setModal(false);
  setWindowTitle("Variable Names");

  QString iconName("");
  if (true) // <-- NEEDS TO BE PROPERLY IMPLEMENTED
    iconName = QString(":/icons/varEdit-dark.svg");
  else
    iconName = QString(":/icons/varEdit-plain.svg");
  setWindowIcon( QIcon( QPixmap(iconName) ) );
}

//--------------------------------------------------------------------------
/**
 * @brief PVarDialog::PVarDialog. Variable GUI allowing the users to define
 * variables.
 * @param collection_list list of all the collections
 * @param darkTheme plain/dark theme flag
 * @param parent parent widget pointer
 * @param f window flag
 */
PVarDialog::PVarDialog(QVector<PCollInfo> collection_list, bool darkTheme,
                       QWidget *parent, Qt::WindowFlags f) :
  QDialog(parent, f), fCollList(collection_list)
{
  fVarEdit = new QPlainTextEdit();
  fCollectionView = new QListWidget();
  fCancel = new QPushButton("&Cancel", this);
  fCheck = new QPushButton("Chec&k", this);
  fAdd = new QPushButton("&Add", this);
  fHelp = new QPushButton("&Help", this);
  fShowVarName = new QPushButton("Show&VarName", this);

  // fill collection view
  for (int i=0; i<fCollList.count(); i++) {
    QListWidgetItem *newItem = new QListWidgetItem;
    newItem->setText(fCollList[i].fCollName);
    fCollectionView->addItem(newItem);
  }
  fCollectionView->setCurrentRow(0, QItemSelectionModel::Select);
  fCollectionView->setSelectionMode(QAbstractItemView::ExtendedSelection);

  QHBoxLayout *hLayout0 = new QHBoxLayout;
  hLayout0->addWidget(fShowVarName);
  hLayout0->addWidget(fHelp);

  QHBoxLayout *hLayout1 = new QHBoxLayout;
  hLayout1->addWidget(fCancel);
  hLayout1->addWidget(fCheck);
  hLayout1->addWidget(fAdd);

  QLabel *varLabel = new QLabel("Edit Variables:");
  QVBoxLayout *varVLayout = new QVBoxLayout;
  varVLayout->addWidget(varLabel);
  varVLayout->addWidget(fVarEdit);

  QLabel *collLabel = new QLabel("Collections:");
  QVBoxLayout *collVLayout = new QVBoxLayout;
  collVLayout->addWidget(collLabel);
  collVLayout->addWidget(fCollectionView);

  QWidget *varWidget = new QWidget(this); // only needed since splitter needs a QWidget
  varWidget->setLayout(varVLayout);
  QWidget *collWidget = new QWidget(this); // only needed since splitter needs a QWidget
  collWidget->setLayout(collVLayout);

  QSplitter *splitter = new QSplitter(Qt::Vertical, this);
  splitter->addWidget(varWidget);
  splitter->addWidget(collWidget);

  QVBoxLayout *vLayout = new QVBoxLayout;
  vLayout->addWidget(splitter);
  vLayout->addLayout(hLayout0);
  vLayout->addLayout(hLayout1);

  fVarEdit->resize(600, 300);
  setLayout(vLayout);
  resize(600, 450);

  connect(fCancel, SIGNAL( clicked() ), this, SLOT( reject() ));
  connect(fCheck, SIGNAL( clicked() ), this, SLOT( check() ));
  connect(fAdd, SIGNAL( clicked() ), this, SLOT( add() ));
  connect(fHelp, SIGNAL( clicked() ), this, SLOT( help() ));
  connect(fShowVarName, SIGNAL( clicked() ), this, SLOT( showVarNames() ));

  QString iconName("");
  if (darkTheme)
    iconName = QString(":/icons/varEdit-dark.svg");
  else
    iconName = QString(":/icons/varEdit-plain.svg");
  setWindowIcon( QIcon( QPixmap(iconName) ) );
}

//--------------------------------------------------------------------------
/**
 * @brief PVarDialog::check. Allows the user to check the variable string.
 */
void PVarDialog::check()
{
  if (!basic_check())
    return;

  if (!var_consistency_check())
    return;

  // create the collection index vector
  QVector<int> idx;
  QList<QListWidgetItem *> selected = fCollectionView->selectedItems();
  for (int i=0; i<selected.count(); i++) {
    idx.push_back(fCollectionView->row(selected[i]));
  }
  emit check_request(fVarEdit->toPlainText(), idx);
}

//--------------------------------------------------------------------------
/**
 * @brief PVarDialog::add. It is used to initiate the add variable to the mupp GUI.
 */
void PVarDialog::add()
{
  if (fVarEdit->toPlainText().isEmpty()) {
    QMessageBox::critical(this, "**ERROR**", "No input available.");
    return;
  }
  if (fCollectionView->selectedItems().count() == 0) {
    QMessageBox::critical(this, "**ERROR**", "One or more collection(s) need to linked to the variable(s).");
    return;
  }

  if (!basic_check())
    return;

  if (!var_consistency_check())
    return;

  // create the collection index vector
  QVector<int> idx;
  QList<QListWidgetItem *> selected = fCollectionView->selectedItems();
  for (int i=0; i<selected.count(); i++) {
    idx.push_back(fCollectionView->row(selected[i]));
  }

  emit add_request(fVarEdit->toPlainText(), idx);
}

//--------------------------------------------------------------------------
/**
 * @brief PVarDialog::help
 */
void PVarDialog::help()
{
  QMessageBox::information(this, "Var Help",
       "Syntax: var <var_name> = <expr>.\n"\
       "<expr> can contain identifiers defined in the collections.\n"\
       "An identifier is an addressed variable which is defined\n"\
       "by a preceeding '$' before the variable name.\n"\
       "Example: variable sigma -> identifier $sigma.\n"\
       "Example:\nvar sigSC = pow(abs(pow($sigma,2.0)-pow(0.11,2.0)),0.5)");
}

//--------------------------------------------------------------------------
/**
 * @brief PVarDialog::showVarNames. Show a variable name dialog for a given
 * selected collection.
 */
void PVarDialog::showVarNames()
{
  // get the selected collection
  if (fCollectionView->selectedItems().count() == 0) {
    QMessageBox::critical(this, "**ERROR**", "At least one collection needs to be selected.");
    return;
  }
  if (fCollectionView->selectedItems().count() > 1) {
    QMessageBox::critical(this, "**ERROR**", "Currently only the vars of a single collection can be shown.");
    return;
  }
  int idx = fCollectionView->currentRow();

  if (idx >= fCollList.count()) {
    QMessageBox::critical(this, "**ERROR**", QString("Collection idx=%1 > #Collections=%2. This never should have happened.").arg(idx).arg(fCollList.count()));
    return;
  }

  PCollInfo info = fCollList[idx];

  PShowVarNameDialog *dialog = new PShowVarNameDialog(info);
  dialog->show();
}

//--------------------------------------------------------------------------
/**
 * @brief PVarDialog::basic_check. Carry out basic checks before feeding it
 * to the parser.
 * @return true on success
 */
bool PVarDialog::basic_check()
{
  QString varStr = fVarEdit->toPlainText();
  bool ok;

  if (varStr.isEmpty()) {
    QMessageBox::critical(this, "**ERROR**", "No input available.");
    return false;
  }
  if (fCollectionView->selectedItems().count() == 0) {
    QMessageBox::critical(this, "**ERROR**", "One or more collection(s) need to linked to the variable(s).");
    return false;
  }

  // tokenize variable input
  QStringList strList = varStr.split(QRegularExpression("\\s+"), QString::SkipEmptyParts);

  // check if there are ANY var definitions
  ok = false;
  for (int i=0; i<strList.count(); i++) {
    if (strList[i] == "var") {
      ok = true;
      break;
    }
  }
  if (!ok) {
    QMessageBox::critical(this, "**ERROR**", "<b>NO</b> 'var' definition found.");
    return false;
  }

  // check that for each variable, there is also an Err variable present
  QStringList varNames = collectVarNames(strList, ok);
  if (!ok) {
    QMessageBox::critical(this, "**ERROR**", "found 'var' without <var_name>.");
    return false;
  }
  QString name("");
  if (!hasErrorDef(varNames, name)) {
    QMessageBox::critical(this, "**ERROR**", QString("found &lt;var_name&gt;=%1 without corresponding %1Err.<br>Both, a variable <b>and</b> its corresponding error variable needs to be defined.").arg(name));
    return false;
  }

  return true;
}

//--------------------------------------------------------------------------
/**
 * @brief PVarDialog::var_consistency_check. Variable consistency checks. For
 * instance it is checked that for each variable there is also the corresponding
 * error variable defined.
 * @return true on success
 */
bool PVarDialog::var_consistency_check()
{
  QString varStr = fVarEdit->toPlainText();

  // collect all identifiers
  int idx = 0, idxEnd = 0;
  QStringList varNames;
  QVector<int> isIdendifier;
  char ch;
  bool done;
  do {
    idx = varStr.indexOf("$", idx);
    if (idx > -1) { // found identifier
      idxEnd = idx+1;
      done = false;
      do {
        ch = varStr[idxEnd].toLatin1();
        if (isalnum(ch) || (ch == '_'))
          idxEnd++;
        else
          done = true;
      } while (!done && (idxEnd <= varStr.length()));
      varNames << varStr.mid(idx+1, idxEnd-idx-1);
      isIdendifier.push_back(1);
      idx++;
    }
  } while (idx != -1);

  // collect all the variable names
  idx = 0;
  do {
    idx = varStr.indexOf("var ", idx);
    if (idx > -1) { // found variable
      idxEnd = varStr.indexOf("=", idx);
      if (idxEnd == -1) {
        return false;
      }
      varNames << varStr.mid(idx+4, idxEnd-idx-5);
      isIdendifier.push_back(0);
      idx++;
    }
  } while (idx != -1);

  // make sure that identifier is found in collection if it is not a variable
  bool isVar = false;
  bool found = false;
  QString str;
  QList<QListWidgetItem *> selColl = fCollectionView->selectedItems();
  for (int i=0; i<varNames.count(); i++) {
    if (isIdendifier[i] == 0) // it is a variable, hence nothing to be done
      continue;
    str = varNames[i];
    if (str.endsWith("Err")) {
      str = str.left(str.indexOf("Err"));
    }
    isVar = false;
    for (int j=i+1; j<varNames.count(); j++) {
      if ((varNames[j] == str) && isIdendifier[j] == 0) {
        isVar = true;
        break;
      }
    }
    if (isVar)
      continue;

    for (int j=0; j<selColl.count(); j++) {
      idx = fCollectionView->row(selColl[j]);
      found = false;
      for (int k=0; k<fCollList[idx].fVarName.count(); k++) {
        if (str == fCollList[idx].fVarName[k]) {
          found = true;
          break;
        }
      }
      if (!found) {
        QMessageBox::critical(this, "**ERROR**", QString("Identifier '%1' not present in selected collection '%2'").arg(str).arg(fCollList[idx].fCollName));
        return false;
      }
    }
  }

  return true;
}

//--------------------------------------------------------------------------
/**
 * @brief PVarDialog::collectVarNames. Collect variable names from edit variable.
 * @param list input list
 * @param ok true if ok
 * @return return variable list
 */
QStringList PVarDialog::collectVarNames(QStringList &list, bool &ok)
{
  QStringList name;

  ok = true;
  for (int i=0; i<list.count(); i++) {
    if (list[i] == "var") {
      if (i+1 >= list.count()) {
        ok = false;
      } else {
        name << list[i+1];
        i++;
      }
    }
  }

  return name;
}

//--------------------------------------------------------------------------
/**
 * @brief PVarDialog::hasErrorDef. Check for error definition
 * @param varNames variable name list
 * @param name failed variable name
 * @return true on success
 */
bool PVarDialog::hasErrorDef(QStringList &varNames, QString &name)
{
  QString errStr;
  for (int i=0; i<varNames.count(); i++) {
    if (!varNames[i].contains("Err", Qt::CaseSensitive)) { // not an error variable
      errStr = varNames[i] + "Err";
      bool ok = false;
      for (int j=0; j<varNames.count(); j++) {
        if (varNames[j] == errStr) {
          ok = true;
          break;
        }
      }
      if (!ok) {
        name = varNames[i];
        return ok;
      }
    }
  }

  return true;
}
