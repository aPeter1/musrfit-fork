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
 * @brief PmuppScript::PmuppScript
 * @param script
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
 * @brief PmuppScript::~PmuppScript
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
 * @brief PmuppScript::executeScript
 * @return
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
      std::cout << "debug> will eventually handle linking of a variable to a collection ..." << std::endl;
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
 * @brief PmuppScript::setLoadPath
 * @param cmd
 */
void PmuppScript::setLoadPath(const QString cmd)
{
  QString str = cmd;
  QStringList tok;

  // remove command from string
  str = str.remove("loadPath ");

  // tokenize path string
  tok = str.split("/", QString::SkipEmptyParts);

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
 * @brief PmuppScript::setSavePath
 * @param cmd
 */
void PmuppScript::setSavePath(const QString cmd)
{
  QString str = cmd;
  QStringList tok;

  // remove command from string
  str = str.remove("savePath ");

  // tokenize path string
  tok = str.split("/", QString::SkipEmptyParts);

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
 * @brief PmuppScript::loadCollection
 * @param str
 * @return
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
 * @brief PmuppScript::select
 * @param str
 * @return
 */
int PmuppScript::select(const QString str)
{
  QString cmd = str;
  QStringList tok = cmd.split(' ', QString::SkipEmptyParts);
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
 * @brief PmuppScript::selectAll
 * @return
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
 * @brief PmuppScript::addX
 * @param str
 * @return
 */
int PmuppScript::addX(const QString str)
{
  QString cmd = str, label;
  QStringList tok = cmd.split(' ', QString::SkipEmptyParts);

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

    // make sure that label is found in ALL collections
    for (int i=0; i<fParamDataHandler->GetNoOfCollections(); i++) {
      coll = fParamDataHandler->GetCollection(i);
      if (!foundLabel(coll, label)) { // label not found
        std::cerr << std::endl << "**ERROR** couldn't find '" << label.toLatin1().constData() << "' in collection '" << coll->GetName().toLatin1().constData() << "'" << std::endl << std::endl;
        return -4;
      }
    }

    // resize fPlotInfo to the number of selections
    fPlotInfo.resize(fParamDataHandler->GetNoOfCollections());

    // feed plot info
    for (int i=0; i<fPlotInfo.size(); i++) {
      fPlotInfo[i].collIdx = i;
      fPlotInfo[i].xLabel = label;
    }
  } else { // a specific selection
    // check that label is found in the selected collection
    coll = fParamDataHandler->GetCollection(fSelected);
    if (coll == 0) {
      std::cerr << std::endl << "**ERROR** in addX: selected collection couldn't be found ..." << std::endl << std::endl;
      return -3;
    }
    if (!foundLabel(coll, label)) { // label not found
      std::cerr << std::endl << "**ERROR** couldn't find '" << label.toLatin1().constData() << "' in collection '" << coll->GetName().toLatin1().constData() << "'" << std::endl << std::endl;
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
 * @brief PmuppScript::addY
 * @param str
 * @return
 */
int PmuppScript::addY(const QString str)
{
  QString cmd = str;
  QVector<QString> label;
  QStringList tok = cmd.split(' ', QString::SkipEmptyParts);

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
    // make sure that label(s) is/are found in ALL collections
    for (int i=0; i<fParamDataHandler->GetNoOfCollections(); i++) {
      coll = fParamDataHandler->GetCollection(i);
      for (int j=0; j<label.size(); j++) {
        if (!foundLabel(coll, label[j])) { // label not found
          std::cerr << std::endl << "**ERROR** couldn't find '" << label[j].toLatin1().constData() << "' in collection '" << coll->GetName().toLatin1().constData() << "'" << std::endl << std::endl;
          return -4;
        }
      }
    }

    // feed plot info with y-label(s)
    for (int i=0; i<fPlotInfo.size(); i++) {
      fPlotInfo[i].yLabel = label;
    }
  } else { // a specific selection
    // clear yLabel
    fPlotEntry.yLabel.clear();

    // check that label is found in the selected collection
    coll = fParamDataHandler->GetCollection(fSelected);
    if (coll == 0) {
      std::cerr << std::endl << "**ERROR** in addY: selected collection couldn't be found ..." << std::endl << std::endl;
      return -3;
    }

    for (int i=0; i<label.size(); i++) {
      if (!foundLabel(coll, label[i])) { // label not found
        std::cerr << std::endl << "**ERROR** couldn't find '" << label[i].toLatin1().constData() << "' in collection '" << coll->GetName().toLatin1().constData() << "'" << std::endl << std::endl;
        return -4;
      }
    }
    fPlotEntry.yLabel = label;

    // add plot entry
    fPlotInfo.push_back(fPlotEntry);
  }


  return 0;
}

//--------------------------------------------------------------------------
/**
 * @brief PmuppScript::plot
 * @param str
 * @return
 */
int PmuppScript::plot(const QString str)
{
  QString cmd = str;
  QStringList tok = cmd.split(' ', QString::SkipEmptyParts);
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
 * @brief PmuppScript::macro
 * @param str
 * @param plotFln
 * @return
 */
int PmuppScript::macro(const QString str, const QString plotFln)
{
  QVector<PmuppMarker> marker = fAdmin->getMarkers();
  QVector<PmuppColor> color = fAdmin->getColors();

  QString cmd = str;
  QStringList tok = cmd.split(' ', QString::SkipEmptyParts);
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

  return 0;
}

//--------------------------------------------------------------------------
/**
 * @brief PmuppScript::var_cmd
 * @param str
 * @return
 */
int PmuppScript::var_cmd(const QString str)
{
  QStringList tok;
  int idx=0;

  // get linked collection index for further use
  tok = str.split(' ', QString::SkipEmptyParts);
  if (tok[1].endsWith("Err")) // error variable no need to do something
    return 0;
  idx = getCollectionIndex(tok[1]);
  if (idx == -1) // var not linked to collection, ignore it
    return 0;

  // check for the related error variable if present
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

  PVarHandler varHandler(fParamDataHandler->GetCollection(idx), parse_str);
  fVarHandler.push_back(varHandler);

  return 0;
}

//--------------------------------------------------------------------------
/**
 * @brief PmuppScript::foundLabel
 * @param coll
 * @return
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
 * @brief PmuppScript::minMax
 * @param min
 * @param max
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
 * @brief PmuppScript::getNicerLabel
 * @param label
 * @return
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
 * @brief PmuppScript::getCollectionIndex
 * @param var_name
 * @return
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
      tok = cmd.split(' ', QString::SkipEmptyParts);
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
