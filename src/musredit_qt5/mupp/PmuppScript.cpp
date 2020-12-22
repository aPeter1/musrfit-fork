/***************************************************************************

  PmuppScript.cpp

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

#include <cmath>
#include <iostream>

#include <QProcessEnvironment>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QProcess>

#include <QtGlobal> // Q_ASSERT

#include "PmuppScript.h"

//--------------------------------------------------------------------------
/**
 * @brief PmuppScript::PmuppScript. Ctor
 * @param script source
 */
PmuppScript::PmuppScript(QStringList script) :
  fScript(script)
{
  fParamDataHandler = 0;
  fLoadPath = QString("./");
  fSavePath = QString("./");
  fSelected = -2; // nothing selected
  fNorm = false;

  fAdmin = new PmuppAdmin();
}

//--------------------------------------------------------------------------
/**
 * @brief PmuppScript::~PmuppScript. Dtor
 */
PmuppScript::~PmuppScript()
{
  if (fParamDataHandler) {
    delete fParamDataHandler;
    fParamDataHandler = 0;
  }

  if (fAdmin) {
    delete fAdmin;
    fAdmin = 0;
  }
}
//--------------------------------------------------------------------------
/**
 * @brief PmuppScript::executeScript. Handles the script commands.
 * @return 0 on success.
 */
int PmuppScript::executeScript()
{
  fParamDataHandler = new PParamDataHandler();
  if (fParamDataHandler == 0) {
    std::cerr << std::endl << "**ERROR** couldn't invoke data handler ..." << std::endl << std::endl;
    return -1;
  }

  QString cmd;
  int status;
  for (int i=0; i<fScript.size(); i++) {
    cmd = fScript.at(i);
    if (cmd.startsWith("loadPath")) {
      setLoadPath(cmd);
    } else if (cmd.startsWith("savePath")) {
      setSavePath(cmd);
    } else if (cmd.startsWith("load ")) {
      status = loadCollection(cmd);
    } else if (cmd.startsWith("selectAll")) {
      status = selectAll();
    } else if (cmd.startsWith("select ")) {
      status = select(cmd);
    } else if (cmd.startsWith("x")) {
      status = addX(cmd);
    } else if (cmd.startsWith("y")) {
      status = addY(cmd);
    } else if (cmd.startsWith("norm")) {
      fNorm = true;
    } else if (cmd.startsWith("plot")) {
      status = plot(cmd);
    } else if (cmd.startsWith("macro")) {
      status = macro(cmd);
    } else if (cmd.startsWith("var")) {
      status = var_cmd(cmd);
    } else if (cmd.startsWith("col")) {
      // nothing to be done here, since var handles it internally
    } else {
      std::cerr << "**ERROR** found unkown script command '" << cmd.toLatin1().constData() << "'." << std::endl << std::endl;
      status = -2;
    }
    // check for errors
    if (status != 0) {
      emit finished();
      return status;
    }
  }

  emit finished();

  return 0;
}

//--------------------------------------------------------------------------
/**
 * @brief PmuppScript::setLoadPath. Sets the load path (where to look for input
 * data.
 * @param cmd set load path command string.
 */
void PmuppScript::setLoadPath(const QString cmd)
{
  QString str = cmd;
  QStringList tok;

  // remove command from string
  str = str.remove("loadPath ");

  // tokenize path string
#if (QT_VERSION < QT_VERSION_CHECK(5, 14, 0))
  tok = str.split("/", QString::SkipEmptyParts);
#else
  tok = str.split("/", Qt::SkipEmptyParts);
#endif

  // check if there is a bash variable which needs to be resolved
  QProcessEnvironment procEnv = QProcessEnvironment::systemEnvironment();
  QString path = QString("");
  for (int i=0; i<tok.size(); i++) {
    str = tok.at(i);
    if (str.startsWith("$")) {
      str = str.remove("$");
      QString var = procEnv.value(str, "");
      path += var + "/";
    } else {
      path += str + "/";
    }
  }

  fLoadPath = path;
}

//--------------------------------------------------------------------------
/**
 * @brief PmuppScript::setSavePath. Sets the save path (where to save the output
 * files.
 * @param cmd save path command string.
 */
void PmuppScript::setSavePath(const QString cmd)
{
  QString str = cmd;
  QStringList tok;

  // remove command from string
  str = str.remove("savePath ");

  // tokenize path string
#if (QT_VERSION < QT_VERSION_CHECK(5, 14, 0))
  tok = str.split("/", QString::SkipEmptyParts);
#else
  tok = str.split("/", Qt::SkipEmptyParts);
#endif

  // check if there is a bash variable which needs to be resolved
  QProcessEnvironment procEnv = QProcessEnvironment::systemEnvironment();
  QString path = QString("");
  for (int i=0; i<tok.size(); i++) {
    str = tok.at(i);
    if (str.startsWith("$")) {
      str = str.remove("$");
      QString var = procEnv.value(str, "");
      path += var + "/";
    } else {
      path += str + "/";
    }
  }

  fSavePath = path;
}

//--------------------------------------------------------------------------
/**
 * @brief PmuppScript::loadCollection. Load collection command
 * @param str load collection command string
 * @return 0 on success.
 */
int PmuppScript::loadCollection(const QString str)
{
  QString fln = str;
  fln = fln.remove("load ");
  fln = fln.trimmed();
  fln = fln.prepend(fLoadPath);

  QStringList flnList;
  flnList << fln;

  QString errorMsg("");
  fParamDataHandler->ReadParamFile(flnList, errorMsg);

  return 0;
}

//--------------------------------------------------------------------------
/**
 * @brief PmuppScript::select. Select collection command.
 * @param str selection command string
 * @return 0 on success
 */
int PmuppScript::select(const QString str)
{
  QString cmd = str;
#if (QT_VERSION < QT_VERSION_CHECK(5, 14, 0))
  QStringList tok = cmd.split(' ', QString::SkipEmptyParts);
#else
  QStringList tok = cmd.split(' ', Qt::SkipEmptyParts);
#endif
  if (tok.size() != 2) {
    std::cerr << std::endl << "**ERROR** wrong 'select' command syntax." << std::endl << std::endl;
    return -1;
  }

  bool ok;
  int ival = tok[1].toInt(&ok);
  if (ok) { // collection index given
    if (ival >= fParamDataHandler->GetNoOfCollections()) {
      std::cerr << std::endl << "**ERROR** try to select a collection with index " << ival << ", which is >= # collections (" << fParamDataHandler->GetNoOfCollections() << ")." << std::endl << std::endl;
      return -2;
    }
    fSelected = ival;
  } else { // assume that a collection name is given
    ival = fParamDataHandler->GetCollectionIndex(tok[1]);
    if (ival == -1) {
      std::cerr << std::endl << "**ERROR** couldn't find collection '" << tok[1].toLatin1().constData() << "'." << std::endl << std::endl;
      return -3;
    }
    if (ival >= fParamDataHandler->GetNoOfCollections()) {
      std::cerr << std::endl << "**ERROR** try to select a collection with index " << ival << ", which is >= # collections (" << fParamDataHandler->GetNoOfCollections() << ")." << std::endl << std::endl;
      return -2;
    }
    fSelected = ival;
  }

  return 0;
}

//--------------------------------------------------------------------------
/**
 * @brief PmuppScript::selectAll. Select all collections
 * @return 0 on success
 */
int PmuppScript::selectAll()
{
  int noColl = fParamDataHandler->GetNoOfCollections();
  if ( noColl > 0) {
    fSelected = -1; // all collections are selected
  } else {
    std::cerr << std::endl << "**ERROR** no collections present, hence it is not possible to select them." << std::endl << std::endl;
    return -1;
  }

  return 0;
}

//--------------------------------------------------------------------------
/**
 * @brief PmuppScript::addX. Add label to x-axis
 * @param str label string
 * @return 0 on success
 */
int PmuppScript::addX(const QString str)
{
  QString cmd = str, label;
#if (QT_VERSION < QT_VERSION_CHECK(5, 14, 0))
  QStringList tok = cmd.split(' ', QString::SkipEmptyParts);
#else
  QStringList tok = cmd.split(' ', Qt::SkipEmptyParts);
#endif

  if (tok.size() != 2) {
    std::cerr << std::endl << "**ERROR** in addX: number of tokens missmatch." << std::endl << std::endl;
    return -1;
  }
  label = tok[1].trimmed();

  PmuppCollection *coll=0;
  if (fSelected == -2) { // no selection -> error
    std::cerr << std::endl << "**ERROR** in addX. addX called without previous 'select' command." << std::endl << std::endl;
    return -2;
  } else if (fSelected == -1) { // i.e. select ALL
    // clean up plot info first
    fPlotInfo.clear();

    // make sure that label is found in ALL collections, or in variables
    // first check collections
    bool foundInColl(true), foundInVar(true);
    QString collName("");
    for (int i=0; i<fParamDataHandler->GetNoOfCollections(); i++) {
      coll = fParamDataHandler->GetCollection(i);
      if (!foundLabel(coll, label)) { // label not found
        foundInColl = false;
        collName = coll->GetName();
        break;
      }
    }
    // second check variables
    if (!foundVariable(label)) { // label not found
      foundInVar = false;
    }
    // make sure label(s) have been found
    if (!foundInColl && !foundInVar) { // not all labels found, neither in collection nor variables.
      std::cerr << std::endl << "**ERROR** couldn't find '" << label.toLatin1().constData() << "' in collection '" << collName.toLatin1().constData() << "'," << std::endl;
      std::cerr << "          nor is it a defined variable" << std::endl << std::endl;
      return -4;
    }

    // resize fPlotInfo to the number of selections
    fPlotInfo.resize(fParamDataHandler->GetNoOfCollections());

    // feed plot info
    for (int i=0; i<fPlotInfo.size(); i++) {
      fPlotInfo[i].collIdx = i;
      fPlotInfo[i].xLabel = label;
    }
  } else { // a specific selection
    // check that label is found in the selected collection, or in variables
    coll = fParamDataHandler->GetCollection(fSelected);
    if (coll == 0) {
      std::cerr << std::endl << "**ERROR** in addX: selected collection couldn't be found ..." << std::endl << std::endl;
      return -3;
    }
    // first check collection
    bool foundInColl(true), foundInVar(true);
    QString collName("");
    if (!foundLabel(coll, label)) { // label not found
      foundInColl = false;
      collName = coll->GetName();
    }
    // second check variables
    if (!foundVariable(label)) { // label not found
      foundInVar = false;
    }
    // make sure label(s) have been found
    if (!foundInColl && !foundInVar) { // not all labels found, neither in collection nor variables.
      std::cerr << std::endl << "**ERROR** couldn't find '" << label.toLatin1().constData() << "' in collection '" << collName.toLatin1().constData() << "'," << std::endl;
      std::cerr << "          nor is it a defined variable" << std::endl << std::endl;
      return -4;
    }

    // feed plot entry
    fPlotEntry.collIdx = fSelected;
    fPlotEntry.xLabel = label;
  }

  return 0;
}

//--------------------------------------------------------------------------
/**
 * @brief PmuppScript::addY. Add label to y-axis.
 * @param str label string
 * @return 0 on success
 */
int PmuppScript::addY(const QString str)
{
  QString cmd = str;
  QVector<QString> label;
#if (QT_VERSION < QT_VERSION_CHECK(5, 14, 0))
  QStringList tok = cmd.split(' ', QString::SkipEmptyParts);
#else
  QStringList tok = cmd.split(' ', Qt::SkipEmptyParts);
#endif

  if (tok.size() < 2) {
    std::cerr << std::endl << "**ERROR** in addY: number of tokens < 2." << std::endl << std::endl;
    return -1;
  }
  // collect all potential labels
  for (int i=1; i<tok.size(); i++)
    label.push_back(tok[i].trimmed());

  PmuppCollection *coll=0;
  if (fSelected == -2) { // no selection -> error
    std::cerr << std::endl << "**ERROR** in addY. addY called without previous 'select' command." << std::endl << std::endl;
    return -2;
  } else if (fSelected == -1) { // i.e. select ALL
    // make sure that label(s) is/are found in ALL collections, or in variables
    // first check collections
    bool foundInColl(true), foundInVar(true);
    int idx = -1;
    QString collName("");
    for (int i=0; i<fParamDataHandler->GetNoOfCollections(); i++) {
      coll = fParamDataHandler->GetCollection(i);
      for (int j=0; j<label.size(); j++) {
        if (!foundLabel(coll, label[j])) { // label not found
          foundInColl = false;
          collName = coll->GetName();
          idx = j;
          break;
        }
      }
    }
    // second check variables
    for (int i=0; i<label.size(); i++) {
      if (!foundVariable(label[i])) { // label not found
        foundInVar = false;
        idx = i;
        break;
      }
    }
    // make sure label(s) have been found
    if (!foundInColl && !foundInVar) { // not all labels found, neither in collection nor variables.
      std::cerr << std::endl << "**ERROR** couldn't find '" << label[idx].toLatin1().constData() << "' in collection '" << collName.toLatin1().constData() << "'," << std::endl;
      std::cerr << "          nor is it a defined variable" << std::endl << std::endl;
      return -4;
    }

    // feed plot info with y-label(s)
    for (int i=0; i<fPlotInfo.size(); i++) {
      fPlotInfo[i].yLabel = label;
    }
  } else { // a specific selection
    // clear yLabel
    fPlotEntry.yLabel.clear();

    // check that label is found in the selected collection, or in the variables
    coll = fParamDataHandler->GetCollection(fSelected);
    if (coll == 0) {
      std::cerr << std::endl << "**ERROR** in addY: selected collection couldn't be found ..." << std::endl << std::endl;
      return -3;
    }

    // first check specific collection
    bool foundInColl(true), foundInVar(true);
    int idx = -1;
    QString collName("");
    for (int i=0; i<label.size(); i++) {
      if (!foundLabel(coll, label[i])) { // label not found
        foundInColl = false;
        collName = coll->GetName();
        idx = i;
        break;
      }
    }
    // second check variables
    for (int i=0; i<label.size(); i++) {
      if (!foundVariable(label[i])) { // label not found
        foundInVar = false;
        idx = i;
        break;
      }
    }
    // make sure label(s) have been found
    if (!foundInColl && !foundInVar) { // not all labels found, neither in collection nor variables.
      std::cerr << std::endl << "**ERROR** couldn't find '" << label[idx].toLatin1().constData() << "' in collection '" << collName.toLatin1().constData() << "'," << std::endl;
      std::cerr << "          nor is it a defined variable" << std::endl << std::endl;
      return -4;
    }

    fPlotEntry.yLabel = label;

    // add plot entry
    fPlotInfo.push_back(fPlotEntry);
  }


  return 0;
}

//--------------------------------------------------------------------------
/**
 * @brief PmuppScript::plot. Create root macro and feed it to root in the bash mode.
 * @param str plot script command string
 * @return 0 on success
 */
int PmuppScript::plot(const QString str)
{
  QString cmd = str;
#if (QT_VERSION < QT_VERSION_CHECK(5, 14, 0))
  QStringList tok = cmd.split(' ', QString::SkipEmptyParts);
#else
  QStringList tok = cmd.split(' ', Qt::SkipEmptyParts);
#endif
  if (tok.size() != 2) {
    std::cerr << std::endl << "**ERROR** in plot: number of tokens != 2." << std::endl << std::endl;
    return -1;
  }
  QString flnOut = fSavePath + tok[1];
  QString macroOut = fSavePath + "__out.C";

  if (macro(QString("macro " + macroOut), flnOut) != 0) {
    std::cerr << std::endl << "**ERROR** temporary macro generation failed." << std::endl << std::endl;
    return -1;
  }

  // call root via batch
  QProcess *proc = new QProcess(this);
  if (proc == nullptr) {
    std::cerr << std::endl << "**ERROR** couldn't invoke root.exe in batch mode." << std::endl << std::endl;
    return -2;
  }

  // make sure that the system environment variables are properly set
  QString exec_cmd = "root.exe";
  QStringList arg;
  arg << "-b";
  arg << macroOut;
  arg << "-q";

  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
  env.insert("LD_LIBRARY_PATH", env.value("ROOTSYS") + "/lib:" + env.value("LD_LIBRARY_PATH"));
  proc->setProcessEnvironment(env);
  proc->setWorkingDirectory(fSavePath);
  proc->start(exec_cmd, arg);
  if (!proc->waitForStarted()) {
    std::cerr << std::endl << "**ERROR** Could not execute the output command: " << exec_cmd.toLatin1().constData() << std::endl << std::endl;
    QFile::remove(macroOut);
    return -3;
  }
  proc->waitForFinished();

  QFile::remove(macroOut);

  return 0;
}

//--------------------------------------------------------------------------
/**
 * @brief PmuppScript::macro. Create a root macro
 * @param str macro string command string
 * @param plotFln plot file name. Depending on the extension (*.png, *.pdf, ...)
 * the given a file will be created.
 * @return 0 on success
 */
int PmuppScript::macro(const QString str, const QString plotFln)
{
  QVector<PmuppMarker> marker = fAdmin->getMarkers();
  QVector<PmuppColor> color = fAdmin->getColors();

  QString cmd = str;
#if (QT_VERSION < QT_VERSION_CHECK(5, 14, 0))
  QStringList tok = cmd.split(' ', QString::SkipEmptyParts);
#else
  QStringList tok = cmd.split(' ', Qt::SkipEmptyParts);
#endif
  if (tok.size() != 2) {
    std::cerr << std::endl << "**ERROR** macro command with wrong number of arguments (" << tok.size() << ")." << std::endl << std::endl;
    return -1;
  }
  QString macroName = tok[1].trimmed();

  QString fln = fSavePath + macroName;
  QFile file(fln);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    std::cerr << std::endl << "**ERROR** Couldn't open macro file for writting." << std::endl << std::endl;
    return -2;
  }

  QTextStream fout(&file);

  // write header
#if (QT_VERSION < QT_VERSION_CHECK(5, 14, 0))
  fout << "// --------------------------" << endl;
  fout << "// " << fln.toLatin1().constData() << endl;
  fout << "// " << QDateTime::currentDateTime().toString("yy/MM/dd - HH:mm:ss") << endl;
  fout << "// --------------------------" << endl;
  fout << "{" << endl;
  fout << "  gROOT->Reset();" << endl;
  fout << endl;
  fout << "  gStyle->SetOptTitle(0);" << endl;
  fout << "  gStyle->SetOptDate(0);" << endl;
  fout << "  gStyle->SetPadColor(TColor::GetColor(255,255,255));    // pad bkg to white" << endl;
  fout << "  gStyle->SetCanvasColor(TColor::GetColor(255,255,255)); // canvas bkg to white" << endl;
  fout << endl;
  fout << "  Int_t nn=0, i=0;" << endl;
  fout << "  Double_t null[512];" << endl;
  fout << "  Double_t xx[512];" << endl;
  fout << "  Double_t yy[512];" << endl;
  fout << "  Double_t yyPosErr[512];" << endl;
  fout << "  Double_t yyNegErr[512];" << endl;
  fout << endl;
#else
  fout << "// --------------------------" << Qt::endl;
  fout << "// " << fln.toLatin1().constData() << Qt::endl;
  fout << "// " << QDateTime::currentDateTime().toString("yy/MM/dd - HH:mm:ss") << Qt::endl;
  fout << "// --------------------------" << Qt::endl;
  fout << "{" << Qt::endl;
  fout << "  gROOT->Reset();" << Qt::endl;
  fout << Qt::endl;
  fout << "  gStyle->SetOptTitle(0);" << Qt::endl;
  fout << "  gStyle->SetOptDate(0);" << Qt::endl;
  fout << "  gStyle->SetPadColor(TColor::GetColor(255,255,255));    // pad bkg to white" << Qt::endl;
  fout << "  gStyle->SetCanvasColor(TColor::GetColor(255,255,255)); // canvas bkg to white" << Qt::endl;
  fout << Qt::endl;
  fout << "  Int_t nn=0, i=0;" << Qt::endl;
  fout << "  Double_t null[512];" << Qt::endl;
  fout << "  Double_t xx[512];" << Qt::endl;
  fout << "  Double_t yy[512];" << Qt::endl;
  fout << "  Double_t yyPosErr[512];" << Qt::endl;
  fout << "  Double_t yyNegErr[512];" << Qt::endl;
  fout << Qt::endl;
#endif

  // write data
  QVector<double> xx, yy, yyPosErr, yyNegErr;
  QString collName;
  int count=0;
  double x_min=0.0, x_max=0.0, x_min_new=0.0, x_max_new=0.0, y_min=0.0, y_max=0.0, y_min_new=0.0, y_max_new=0.0;
  double dval, y_min_norm=1.0e10;
  for (int i=0; i<fPlotInfo.size(); i++) {
    // get collection name
    collName = fParamDataHandler->GetCollectionName(i);
    xx = fParamDataHandler->GetValues(collName, fPlotInfo[i].xLabel);
    if (xx.size() == 0) { // it is a variable
      int idx = getVarIndex(fPlotInfo[i].xLabel);
      if (idx == -1) {
        std::cerr << std::endl;
        std::cerr << "**ERROR** Couldn't get x-label '" << fPlotInfo[i].xLabel.toLatin1().data() << "'." << std::endl;
        std::cerr << "          This should never happens." << std::endl;
        return -3;
      }
#if (QT_VERSION < QT_VERSION_CHECK(5, 14, 0))
      xx = QVector<double>::fromStdVector(fVarHandler[idx].getValues());
#else
      QVector<double> qvec(fVarHandler[idx].getValues().begin(), fVarHandler[idx].getValues().end());
      xx = qvec;
#endif
    }
    // get x-axis min/max
    minMax(xx, x_min, x_max);
    if (count==0) {
      x_min_new = x_min;
      x_max_new = x_max;
    } else {
      if (x_min < x_min_new)
        x_min_new = x_min;
      if (y_max > x_max_new)
        x_max_new = x_max;
    }

    for (int j=0; j<fPlotInfo[i].yLabel.size(); j++) {
      yy = fParamDataHandler->GetValues(collName, fPlotInfo[i].yLabel[j]);
      yyPosErr = fParamDataHandler->GetPosErr(collName, fPlotInfo[i].yLabel[j]);
      yyNegErr = fParamDataHandler->GetNegErr(collName, fPlotInfo[i].yLabel[j]);
      if (yy.size() == 0) { // it's a variable
        int idx = getVarIndex(fPlotInfo[i].yLabel[j]);
        if (idx == -1) {
          std::cerr << std::endl;
          std::cerr << "**ERROR** Couldn't get y-label '" << fPlotInfo[i].yLabel[j].toLatin1().data() << "'." << std::endl;
          std::cerr << "          This should never happens." << std::endl;
          return -3;
        }
#if (QT_VERSION < QT_VERSION_CHECK(5, 14, 0))
        yy = QVector<double>::fromStdVector(fVarHandler[idx].getValues());
        yyPosErr = QVector<double>::fromStdVector(fVarHandler[idx].getErrors());
        yyNegErr = QVector<double>::fromStdVector(fVarHandler[idx].getErrors());
#else
        QVector<double> qvecV(fVarHandler[idx].getValues().begin(), fVarHandler[idx].getValues().end());
        yy = qvecV;
        QVector<double> qvecE(fVarHandler[idx].getErrors().begin(), fVarHandler[idx].getErrors().end());
        yyPosErr = qvecE;
        yyNegErr = qvecE;
#endif
      }
      // get y-axis min/max
      minMax(yy, y_min, y_max);
      if (count==0) {
        y_min_new = y_min;
        y_max_new = y_max;
      } else {
        if (y_min < y_min_new)
          y_min_new = y_min;
        if (y_max > y_max_new)
          y_max_new = y_max;
      }
#if (QT_VERSION < QT_VERSION_CHECK(5, 14, 0))
      fout << "  // " << ++count << ". data set" << endl;
      fout << "  nn = " << xx.size() << ";" << endl;
      fout << "  // null-values" << endl;
      for (int k=0; k<xx.size(); k++) {
        fout << "  null[" << k << "]=0.0;" << endl;
      }
      fout << "  // x-values" << endl;
      for (int k=0; k<xx.size(); k++) {
        fout << "  xx[" << k << "]=" << xx[k] << ";" << endl;
      }
      fout << "  // y-values" << endl;
      for (int k=0; k<yy.size(); k++) {
        dval = yy[k];
        if (fNorm) {
          dval /= y_max;
          if (dval < y_min_norm)
            y_min_norm = dval;
        }
        fout << "  yy[" << k << "]=" << dval << ";" << endl;
      }
      fout << "  // yyNegErr-values" << endl;
      for (int k=0; k<yyNegErr.size(); k++) {
        dval = fabs(yyNegErr[k]);
        if (fNorm)
          dval /= fabs(y_max);
        fout << "  yyNegErr[" << k << "]=" << dval << ";" << endl;
      }
      fout << "  // yyPosErr-values" << endl;
      for (int k=0; k<yyPosErr.size(); k++) {
        dval = fabs(yyPosErr[k]);
        if (fNorm)
          dval /= fabs(y_max);
        fout << "  yyPosErr[" << k << "]=" << dval << ";" << endl;
      }
      fout << endl;
      fout << "  TGraphAsymmErrors *g_" << i << "_" << j << " = new TGraphAsymmErrors(nn, xx, yy, null, null, yyNegErr, yyPosErr);" << endl;
      fout << endl;
#else
      fout << "  // " << ++count << ". data set" << Qt::endl;
      fout << "  nn = " << xx.size() << ";" << Qt::endl;
      fout << "  // null-values" << Qt::endl;
      for (int k=0; k<xx.size(); k++) {
        fout << "  null[" << k << "]=0.0;" << Qt::endl;
      }
      fout << "  // x-values" << Qt::endl;
      for (int k=0; k<xx.size(); k++) {
        fout << "  xx[" << k << "]=" << xx[k] << ";" << Qt::endl;
      }
      fout << "  // y-values" << Qt::endl;
      for (int k=0; k<yy.size(); k++) {
        dval = yy[k];
        if (fNorm) {
          dval /= y_max;
          if (dval < y_min_norm)
            y_min_norm = dval;
        }
        fout << "  yy[" << k << "]=" << dval << ";" << Qt::endl;
      }
      fout << "  // yyNegErr-values" << Qt::endl;
      for (int k=0; k<yyNegErr.size(); k++) {
        dval = fabs(yyNegErr[k]);
        if (fNorm)
          dval /= fabs(y_max);
        fout << "  yyNegErr[" << k << "]=" << dval << ";" << Qt::endl;
      }
      fout << "  // yyPosErr-values" << Qt::endl;
      for (int k=0; k<yyPosErr.size(); k++) {
        dval = fabs(yyPosErr[k]);
        if (fNorm)
          dval /= fabs(y_max);
        fout << "  yyPosErr[" << k << "]=" << dval << ";" << Qt::endl;
      }
      fout << Qt::endl;
      fout << "  TGraphAsymmErrors *g_" << i << "_" << j << " = new TGraphAsymmErrors(nn, xx, yy, null, null, yyNegErr, yyPosErr);" << Qt::endl;
      fout << Qt::endl;
#endif
    }
  }

  // x,y-min/max range
  double diff;
  if (x_min_new > 0.0)
    x_min = 0.0;
  else
    x_min = x_min_new;
  diff = x_max-x_min;
  x_max = x_max_new + 0.05*diff;  
  diff = y_max_new - y_min_new;
  y_min = y_min_new - 0.05 * diff;
  y_max = y_max_new + 0.05 * diff;
  if (fNorm) {
    diff = 1.0 - y_min_norm;
    y_min = y_min_norm - 0.05 * diff;
    y_max = 1.0 + 0.05 * diff;
  }

  // plotting
#if (QT_VERSION < QT_VERSION_CHECK(5, 14, 0))
  fout << "  //**********" << endl;
  fout << "  // plotting " << endl;
  fout << "  //**********" << endl;
  fout << "  TCanvas *c1 = new TCanvas(\"c1\", \"" << macroName.toLatin1().constData() << "\", 10, 10, 600, 700);" << endl;
  fout << endl;

  count = 0;
  int rr, gg, bb;
  for (int i=0; i<fPlotInfo.size(); i++) {
    for (int j=0; j<fPlotInfo[i].yLabel.size(); j++) {
      if (count == 0) {
        if (count < marker.size()) {
          fout << "  g_" << i << "_" << j << "->SetMarkerStyle(" << marker[count].getMarker() << ");" << endl;
          fout << "  g_" << i << "_" << j << "->SetMarkerSize(" << marker[count].getMarkerSize() << ");" << endl;
          color[count].getRGB(rr, gg, bb);
          fout << "  g_" << i << "_" << j << "->SetMarkerColor(TColor::GetColor(" << rr << "," << gg << "," << bb << "));" << endl;
          fout << "  g_" << i << "_" << j << "->SetLineColor(TColor::GetColor(" << rr << "," << gg << "," << bb << "));" << endl;
        } else {
          fout << "  g_" << i << "_" << j << "->SetMarkerStyle(20);" << endl;
          fout << "  g_" << i << "_" << j << "->SetMarkerSize(1.3);" << endl;
          fout << "  g_" << i << "_" << j << "->SetMarkerColor(TColor::GetColor(0,0,0));" << endl;
          fout << "  g_" << i << "_" << j << "->SetLineColor(TColor::GetColor(0,0,0));" << endl;
        }
        fout << "  g_" << i << "_" << j << "->SetFillColor(kWhite);" << endl;
        fout << "  g_" << i << "_" << j << "->GetXaxis()->SetTitle(\"" << getNicerLabel(fPlotInfo[0].xLabel).toLatin1().data() << "\");" << endl;
        fout << "  g_" << i << "_" << j << "->GetXaxis()->SetTitleSize(0.05);" << endl;
        fout << "  g_" << i << "_" << j << "->GetXaxis()->SetDecimals(kTRUE);" << endl;
        fout << "  g_" << i << "_" << j << "->GetXaxis()->SetLimits(" << x_min << "," << x_max << ");" << endl;
        fout << "  g_" << i << "_" << j << "->GetYaxis()->SetTitle(\"" << getNicerLabel(fPlotInfo[0].yLabel[0]).toLatin1().data() << "\");" << endl;
        fout << "  g_" << i << "_" << j << "->GetYaxis()->SetTitleSize(0.05);" << endl;
        fout << "  g_" << i << "_" << j << "->GetYaxis()->SetTitleOffset(1.30);" << endl;
        fout << "  g_" << i << "_" << j << "->GetYaxis()->SetRangeUser(" << y_min << "," << y_max << ");" << endl;
        fout << "  g_" << i << "_" << j << "->Draw(\"AP\");" << endl;
      } else {
        if (count < marker.size()) {
          fout << "  g_" << i << "_" << j << "->SetMarkerStyle(" << marker[count].getMarker() << ");" << endl;
          fout << "  g_" << i << "_" << j << "->SetMarkerSize(" << marker[count].getMarkerSize() << ");" << endl;
          color[count].getRGB(rr, gg, bb);
          fout << "  g_" << i << "_" << j << "->SetMarkerColor(TColor::GetColor(" << rr << "," << gg << "," << bb << "));" << endl;
          fout << "  g_" << i << "_" << j << "->SetLineColor(TColor::GetColor(" << rr << "," << gg << "," << bb << "));" << endl;
        } else {
          fout << "  g_" << i << "_" << j << "->SetMarkerStyle(20);" << endl;
          fout << "  g_" << i << "_" << j << "->SetMarkerSize(1.3);" << endl;
          fout << "  g_" << i << "_" << j << "->SetMarkerColor(TColor::GetColor(0,0,0));" << endl;
          fout << "  g_" << i << "_" << j << "->SetLineColor(TColor::GetColor(0,0,0));" << endl;
        }
        fout << "  g_" << i << "_" << j << "->SetFillColor(kWhite);" << endl;
        fout << "  g_" << i << "_" << j << "->Draw(\"Psame\");" << endl;
      }
      count++;
    }
  }
  fout << "  c1->SetMargin(0.15, 0.05, 0.12, 0.05);" << endl;
  fout << "  c1->Update();" << endl;
  if (!plotFln.isEmpty()) {
    fout << endl;
    fout << "  c1->SaveAs(\"" << plotFln.toLatin1().constData() << "\");" << endl;
  }
  fout << "}" << endl;
#else
  fout << "  //**********" << Qt::endl;
  fout << "  // plotting " << Qt::endl;
  fout << "  //**********" << Qt::endl;
  fout << "  TCanvas *c1 = new TCanvas(\"c1\", \"" << macroName.toLatin1().constData() << "\", 10, 10, 600, 700);" << Qt::endl;
  fout << Qt::endl;

  count = 0;
  int rr, gg, bb;
  for (int i=0; i<fPlotInfo.size(); i++) {
    for (int j=0; j<fPlotInfo[i].yLabel.size(); j++) {
      if (count == 0) {
        if (count < marker.size()) {
          fout << "  g_" << i << "_" << j << "->SetMarkerStyle(" << marker[count].getMarker() << ");" << Qt::endl;
          fout << "  g_" << i << "_" << j << "->SetMarkerSize(" << marker[count].getMarkerSize() << ");" << Qt::endl;
          color[count].getRGB(rr, gg, bb);
          fout << "  g_" << i << "_" << j << "->SetMarkerColor(TColor::GetColor(" << rr << "," << gg << "," << bb << "));" << Qt::endl;
          fout << "  g_" << i << "_" << j << "->SetLineColor(TColor::GetColor(" << rr << "," << gg << "," << bb << "));" << Qt::endl;
        } else {
          fout << "  g_" << i << "_" << j << "->SetMarkerStyle(20);" << Qt::endl;
          fout << "  g_" << i << "_" << j << "->SetMarkerSize(1.3);" << Qt::endl;
          fout << "  g_" << i << "_" << j << "->SetMarkerColor(TColor::GetColor(0,0,0));" << Qt::endl;
          fout << "  g_" << i << "_" << j << "->SetLineColor(TColor::GetColor(0,0,0));" << Qt::endl;
        }
        fout << "  g_" << i << "_" << j << "->SetFillColor(kWhite);" << Qt::endl;
        fout << "  g_" << i << "_" << j << "->GetXaxis()->SetTitle(\"" << getNicerLabel(fPlotInfo[0].xLabel).toLatin1().data() << "\");" << Qt::endl;
        fout << "  g_" << i << "_" << j << "->GetXaxis()->SetTitleSize(0.05);" << Qt::endl;
        fout << "  g_" << i << "_" << j << "->GetXaxis()->SetDecimals(kTRUE);" << Qt::endl;
        fout << "  g_" << i << "_" << j << "->GetXaxis()->SetLimits(" << x_min << "," << x_max << ");" << Qt::endl;
        fout << "  g_" << i << "_" << j << "->GetYaxis()->SetTitle(\"" << getNicerLabel(fPlotInfo[0].yLabel[0]).toLatin1().data() << "\");" << Qt::endl;
        fout << "  g_" << i << "_" << j << "->GetYaxis()->SetTitleSize(0.05);" << Qt::endl;
        fout << "  g_" << i << "_" << j << "->GetYaxis()->SetTitleOffset(1.30);" << Qt::endl;
        fout << "  g_" << i << "_" << j << "->GetYaxis()->SetRangeUser(" << y_min << "," << y_max << ");" << Qt::endl;
        fout << "  g_" << i << "_" << j << "->Draw(\"AP\");" << Qt::endl;
      } else {
        if (count < marker.size()) {
          fout << "  g_" << i << "_" << j << "->SetMarkerStyle(" << marker[count].getMarker() << ");" << Qt::endl;
          fout << "  g_" << i << "_" << j << "->SetMarkerSize(" << marker[count].getMarkerSize() << ");" << Qt::endl;
          color[count].getRGB(rr, gg, bb);
          fout << "  g_" << i << "_" << j << "->SetMarkerColor(TColor::GetColor(" << rr << "," << gg << "," << bb << "));" << Qt::endl;
          fout << "  g_" << i << "_" << j << "->SetLineColor(TColor::GetColor(" << rr << "," << gg << "," << bb << "));" << Qt::endl;
        } else {
          fout << "  g_" << i << "_" << j << "->SetMarkerStyle(20);" << Qt::endl;
          fout << "  g_" << i << "_" << j << "->SetMarkerSize(1.3);" << Qt::endl;
          fout << "  g_" << i << "_" << j << "->SetMarkerColor(TColor::GetColor(0,0,0));" << Qt::endl;
          fout << "  g_" << i << "_" << j << "->SetLineColor(TColor::GetColor(0,0,0));" << Qt::endl;
        }
        fout << "  g_" << i << "_" << j << "->SetFillColor(kWhite);" << Qt::endl;
        fout << "  g_" << i << "_" << j << "->Draw(\"Psame\");" << Qt::endl;
      }
      count++;
    }
  }
  fout << "  c1->SetMargin(0.15, 0.05, 0.12, 0.05);" << Qt::endl;
  fout << "  c1->Update();" << Qt::endl;
  if (!plotFln.isEmpty()) {
    fout << Qt::endl;
    fout << "  c1->SaveAs(\"" << plotFln.toLatin1().constData() << "\");" << Qt::endl;
  }
  fout << "}" << Qt::endl;
#endif

  return 0;
}

//--------------------------------------------------------------------------
/**
 * @brief PmuppScript::var_cmd. Variable definition command
 * @param str variable definition command string
 * @return 0 on success
 */
int PmuppScript::var_cmd(const QString str)
{
  QStringList tok;
  int idx=0;

  // get linked collection index for further use
#if (QT_VERSION < QT_VERSION_CHECK(5, 14, 0))
  tok = str.split(' ', QString::SkipEmptyParts);
#else
  tok = str.split(' ', Qt::SkipEmptyParts);
#endif
  if (tok[1].endsWith("Err")) // error variable no need to do something
    return 0;
  idx = getCollectionIndex(tok[1]);
  if (idx == -1) // var not linked to collection, ignore it
    return 0;

  // check if the related error variable is present
  QString varErr = QString("%1%2").arg(tok[1]).arg("Err");
  QString varErrCmd("");
  for (int i=0; i<fScript.size(); i++) {
    if (fScript.at(i).contains(varErr, Qt::CaseSensitive)) {
      varErrCmd = fScript.at(i);
      break;
    }
  }

  std::string parse_str = str.toLatin1().data();
  if (!varErrCmd.isEmpty()) {
    parse_str += "\n";
    parse_str += varErrCmd.toLatin1().data();
  }

  PVarHandler varHandler(fParamDataHandler->GetCollection(idx), parse_str, tok[1].toLatin1().data());
  if (!varHandler.isValid()) {
    // deal with errors
    QString mupp_err = QString("%1/.musrfit/mupp/mupp_err.log").arg(QString(qgetenv("HOME")));

    // dump error messages if present
    QFile fout(mupp_err);
    if (fout.open(QIODevice::ReadOnly | QIODevice::Text)) {
      QString msg;
      while (!fout.atEnd()) {
        msg = fout.readLine();
        std::cerr << msg.toLatin1().data();
      }
      // delete potentially present mupp_err.log file
      QFile::remove(mupp_err);
    }
    return 1;
  }
  fVarHandler.push_back(varHandler);

  return 0;
}

//--------------------------------------------------------------------------
/**
 * @brief PmuppScript::foundLabel find label in collection coll.
 * @param coll collection object
 * @return true if found
 */
bool PmuppScript::foundLabel(PmuppCollection *coll, const QString label)
{
  bool result = false;
  for (int i=0; i<coll->GetRun(0).GetNoOfParam(); i++) {
    if (!coll->GetRun(0).GetParam(i).GetName().compare(label)) {
      result = true;
      break;
    }
  }

  return result;
}

//--------------------------------------------------------------------------
/**
 * @brief PmuppScript::foundVariable. Check if a variable is found in the
 * variable handler.
 * @param var variable name
 * @return true if found
 */
bool PmuppScript::foundVariable(const QString var)
{
  bool result = false;
  for (int i=0; i<fVarHandler.size(); i++) {
    if (!fVarHandler[i].getVarName().compare(var)) {
      result = true;
      break;
    }
  }

  return result;
}

//--------------------------------------------------------------------------
/**
 * @brief PmuppScript::getVarIndex. Get index of the variable object given by
 * its name.
 * @param var variable name searched for
 * @return idx of found, -1 otherwise
 */
int PmuppScript::getVarIndex(const QString var)
{
  int idx = -1;
  for (int i=0; i<fVarHandler.size(); i++) {
    if (!fVarHandler[i].getVarName().compare(var)) {
      idx = i;
      break;
    }
  }

  return idx;
}

//--------------------------------------------------------------------------
/**
 * @brief PmuppScript::minMax get minimum and maximum of the data set.
 * @param dvec data set vector
 * @param min resulting minimum
 * @param max resulting maximum
 */
void PmuppScript::minMax(QVector<double> dvec, double &min, double &max)
{
  min = 99.0e6;
  max = -99.0e6;
  for (int i=0; i<dvec.size(); i++) {
    if (dvec[i] < min)
      min = dvec[i];
    if (dvec[i] > max)
      max = dvec[i];
  }
}

//--------------------------------------------------------------------------
/**
 * @brief PmuppScript::getNicerLabel. Prettify the labels.
 * @param label to be prettified
 * @return prettified label
 */
QString PmuppScript::getNicerLabel(const QString label)
{
  QString nice = label;

  if (label == "dataE") {
    nice = "E (keV)";
  } else if (label == "dataT") {
    nice = "T (K)";
  } else if (label == "dataB") {
    nice = "B (G)";
  } else if (!label.compare("sigma", Qt::CaseInsensitive)) {
    if (fNorm)
      nice = "#sigma/max(#sigma)";
    else
      nice = "#sigma (1/#mus)";
  } else if (!label.compare("lambda", Qt::CaseInsensitive)) {
    if (fNorm)
      nice = "#lambda/max(#lambda)";
    else
      nice = "#lambda (1/#mus)";
  } else if (!label.compare("rate", Qt::CaseInsensitive)) {
    if (fNorm)
      nice = "Rate/max(Rate)";
    else
      nice = "Rate (1/#mus)";
  } else if (!label.compare("alpha_LR", Qt::CaseInsensitive)) {
    if (fNorm)
      nice = "#alpha_{LR}/max(#alpha_{LR})";
    else
      nice = "#alpha_{LR}";
  } else if (!label.compare("alpha_TB", Qt::CaseInsensitive)) {
    if (fNorm)
      nice = "#alpha_{TB}/max(#alpha_{TB})";
    else
      nice = "#alpha_{TB}";
  } else {
    if (fNorm) {
      nice = label + "/ max(" + label + ")";
    }
  }

  return nice;
}

//--------------------------------------------------------------------------
/**
 * @brief PmuppScript::getCollectionIndex. Get the index of a collection.
 * @param var_name name of the collection
 * @return idx on success, -1 otherwise
 */
int PmuppScript::getCollectionIndex(const QString var_name)
{
  int idx = -1;
  QString cmd;
  QStringList tok;
  bool ok;

  for (int i=0; i<fScript.size(); i++) {
    cmd = fScript.at(i);
    if (cmd.startsWith("col")) {
      tok.clear();
#if (QT_VERSION < QT_VERSION_CHECK(5, 14, 0))
      tok = cmd.split(' ', QString::SkipEmptyParts);
#else
      tok = cmd.split(' ', Qt::SkipEmptyParts);
#endif
      if (tok[3] == var_name) {
        idx = tok[1].toInt(&ok);
        if (!ok) {
          Q_ASSERT(0);
        }
        break;
      }
    }
  }

  return idx;
}
