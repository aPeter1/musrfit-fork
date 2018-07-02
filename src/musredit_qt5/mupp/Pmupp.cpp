/***************************************************************************

  Pmupp.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007-2017 by Andreas Suter                              *
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
#include <cmath>
using namespace std;

#include <QFile>
#include <QTextStream>
#include <QString>
#include <QStringList>
#include <QInputDialog>
#include <QDir>
#include <QLineEdit>
#include <QMessageBox>
#include <QProcess>
#include <QProcessEnvironment>

#include <QtDebug>

#include "Pmupp.h"

#define PMUPP_UNDEF    -1
#define PMUPP_VALUE     0
#define PMUPP_POSNEGERR 1
#define PMUPP_POSERR    2
#define PMUPP_NEGERR    3
#define PMUPP_RUN       4

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * @brief PmuppParam::PmuppParam
 */
PmuppParam::PmuppParam() {
  ResetParam();
}

//--------------------------------------------------------------------------
/**
 * @brief PmuppParam::PmuppParam
 * @param name
 * @param param
 * @param posErr
 */
PmuppParam::PmuppParam(QString name, double param, double posErr)
{
  SetParam(name, param, posErr);
}

//--------------------------------------------------------------------------
/**
 * @brief PmuppParam::PmuppParam
 * @param name
 * @param param
 * @param posErr
 * @param negErr
 */
PmuppParam::PmuppParam(QString name, double param, double posErr, double negErr)
{
  SetParam(name, param, posErr, negErr);
}

//--------------------------------------------------------------------------
/**
 * @brief PmuppParam::ResetParam
 */
void PmuppParam::ResetParam()
{
  fName = "";
  fValue = MUPP_UNDEF;
  fPosErr = MUPP_UNDEF;
  fNegErr = MUPP_UNDEF;
}

//--------------------------------------------------------------------------
/**
 * @brief PmuppParam::SetParam
 * @param name
 * @param param
 * @param posErr
 */
void PmuppParam::SetParam(QString name, double param, double posErr)
{
  fName = name;
  fValue = param;
  fPosErr = posErr;
}

//--------------------------------------------------------------------------
/**
 * @brief PmuppParam::SetParam
 * @param name
 * @param param
 * @param posErr
 * @param negErr
 */
void PmuppParam::SetParam(QString name, double param, double posErr, double negErr)
{
  fName = name;
  fValue = param;
  fPosErr = posErr;
  fNegErr = negErr;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * @brief PmuppRun::GetParam
 * @param idx
 * @return
 */
PmuppParam PmuppRun::GetParam(unsigned int idx)
{
  PmuppParam param;

  if (idx < (unsigned int)fParam.size())
    param = fParam[idx];

  return param;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * @brief PmuppCollection::GetRun
 * @param idx
 * @return
 */
PmuppRun PmuppCollection::GetRun(unsigned int idx)
{
  PmuppRun run;

  if (idx < (unsigned int)fRun.size())
    run = fRun[idx];

  return run;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * @brief PParamDataHandler::NewCollection
 * @param name
 */
void PParamDataHandler::NewCollection(const QString name)
{
  PmuppCollection collection;
  collection.SetName(name);

  fCollection.push_back(collection);
}

//--------------------------------------------------------------------------
/**
 * @brief PParamDataHandler::analyzeFileList
 * @param fln
 * @param collectionName
 * @param arg
 * @param workDir
 * @param errorMsg
 * @return
 */
bool PParamDataHandler::analyzeFileList(const QStringList &fln, QString &collectionName, QStringList &arg, QString &workDir, QString &errorMsg)
{
  // 1) check all the msr-files have the same structure: <runNo><extension>.msr with <extension> the same
  //    for all msr-files present.
  QString ext("");
  QStringList run;

  QStringList tok;
  int pos0=-1, pos1=-1;
  QString flnCurrent(""), extCurrent(""), runStr("");
  bool ok;

  pos0 = fln[0].lastIndexOf("/");
  workDir = fln[0].left(pos0);

  for (int i=0; i<fln.count(); i++) {
    // get file name from pathName
    tok = fln[i].split("/");
    flnCurrent = tok[tok.size()-1];

    // get index of position between <run> and <extenstion>
    pos0 = flnCurrent.indexOf("_");
    if (pos0 == -1) {
      errorMsg = "msr-file name has a structure which cannot be handled.\n\
                  It should be <run><extension>, where <run> is the run number\n\
                  and <extension> needs to start with a '_'.";
       return false;
    }
    pos1 = flnCurrent.lastIndexOf(".");
    if ((pos1 == -1) || (pos1 < pos0)) {
      errorMsg = "msr-file name has a structure which cannot be handled.\n\
                  It should be <run><extension>.msr, where <run> is the run number\n\
                  and <extension> needs to start with a '_'.";
       return false;
    }

    // get run number
    runStr = flnCurrent.left(pos0);
    runStr.toInt(&ok); // output not needed, only check that it is a number
    if (!ok) {
      errorMsg = QString("Found run number string '%1' which is not a number.").arg(runStr);
      return false;
    }
    run << runStr;

    // keep extension
    if (i == 0)
      ext = flnCurrent.mid(pos0, pos1-pos0);
    else
      extCurrent = flnCurrent.mid(pos0, pos1-pos0);

    // make sure all extensions are identical
    if ((i>0) && (ext != extCurrent)) {
      errorMsg = "Currently mixed msr-file extensions cannot be handled.";
      return false;
    }
  }

  arg << "[";
  for (int i=0; i<run.size(); i++)
    arg << run[i];
  arg << "]";
  arg << ext;
  arg << "-o";
  arg << collectionName;

  return true;
}

//--------------------------------------------------------------------------
/**
 * @brief PParamDataHandler::ReadParamFile
 * @param fln
 * @param errorMsg
 * @return
 */
bool PParamDataHandler::ReadParamFile(const QStringList fln, QString &errorMsg)
{
  bool valid = true;
  PmuppRun run;
  PmuppCollection collection;
  QString collName("");

  // msr-file list
  if (fln[0].endsWith(".msr")) {
    bool ok=false;
    collName = QInputDialog::getText(0, "Get Collection Name", "Please enter a collection name",
                                             QLineEdit::Normal, "coll007", &ok);
    if (!ok || collName.isEmpty())
      return false;

    // add necessary extension
    collName += ".db";

    // analyse file name list to get the appropriate parts for msr2data
    QStringList arg;
    QString workDir("./");
    if (!analyzeFileList(fln, collName, arg, workDir, errorMsg))
      return false;

    // make sure that the system environment variables are properly set
    QString cmd("");
    fProc = new QProcess(this);
    connect( fProc, SIGNAL( readyReadStandardOutput() ), this, SLOT( readFromStdOut() ) );
    connect( fProc, SIGNAL( readyReadStandardError() ), this, SLOT( readFromStdErr() ) );
    connect( fProc, SIGNAL( finished(int, QProcess::ExitStatus) ), this, SLOT( processDone(int, QProcess::ExitStatus) ) );

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("LD_LIBRARY_PATH", env.value("ROOTSYS") + "/lib:" + env.value("LD_LIBRARY_PATH"));
    cmd = env.value("MUSRFITPATH") + QString("/msr2data");
    if (!QFile::exists(cmd)) { // MUSRFITPATH not set?
      // try ROOTSYS
      cmd = env.value("ROOTSYS") + QString("/bin/msr2data");
      if (!QFile::exists(cmd)) {
        errorMsg = "cannot find msr2data need here.";
        return false;
      }
    }
    QString pathName = workDir + QString("/") + collName;
    if (QFile::exists(pathName)) // if collection already exists
      QFile::remove(pathName);   // delete it
    fProc->setProcessEnvironment(env);
    fProc->setWorkingDirectory(workDir);
    fProc->start(cmd, arg);
    if (!fProc->waitForFinished()) {
      errorMsg = QString(tr("Could not execute the output command: ")+cmd[0]);
      return false;
    }

    // since the db-file should now be present, just load it
    collection = ReadDbFile(pathName, valid, errorMsg);
    if (!valid) {      
      cerr << endl;
      cerr << "----" << endl;
      cerr << "**ERROR** read db-file failure (" << pathName.toLatin1().data() << "." << endl;
      cerr << "----" << endl;
      return false;
    }
    collName.remove(".db");
    collection.SetPathName(pathName);
    collection.SetName(collName);
    fCollection.push_back(collection);
  } else { // db-, dat-file list
    for (int i=0; i<fln.size(); i++) {
      if (fln[i].endsWith(".db")) {
        collection = ReadDbFile(fln[i], valid, errorMsg);
        if (!valid) {
          return false;
        }
        if (!fln[i].startsWith("/")) { // file name only, or relative path
          if (fln[i].startsWith("..")) { // relative path
            int idx = fln[i].lastIndexOf("/");
            if (idx == -1) { // should never happen
              errorMsg = QString("found '%1' which shouldn't be possible!").arg(fln[i]);
              cerr << endl;
              cerr << "----" << endl;
              cerr << "**ERROR** " << errorMsg.toLatin1().data() << endl;
              cerr << "----" << endl;
              return false;
            }
            QString relPath = fln[i].left(idx);
            QString fileName = fln[i].right(fln[i].length()-idx-1);
            QDir dir(relPath);
            collection.SetPathName(dir.absolutePath()+ "/" +fileName);
          } else { // file name only
            QDir dir(QDir::currentPath());
            collection.SetPathName(dir.path() + "/" + fln[i]);
          }
        } else {
          collection.SetPathName(fln[i]);
        }
        int pos = fln[i].lastIndexOf("/");
        collName = "";
        if (pos == -1)
          collName = fln[i];
        else
          collName = fln[i].right(fln[i].length()-pos-1);
        collection.SetName(collName);
        fCollection.push_back(collection);
      } else if (fln[i].endsWith(".dat") || fln[i].endsWith(".txt")) {
        collection = ReadColumnParamFile(fln[i], valid, errorMsg);
        if (!valid) {
          return false;
        }
        fCollection.push_back(collection);
      } else {
        errorMsg = QString("unkown file type for ")+fln[i];
        cerr << endl;
        cerr << "*********" << endl;
        cerr << "**ERROR** " << errorMsg.toLatin1().data() << endl;
        cerr << "*********" << endl;
        return false;
      }
    }
  }

  if (valid)
    emit newData();

  return valid;
}

//--------------------------------------------------------------------------
/**
 * @brief PParamDataHandler::ReadDbFile
 * @param fln
 * @param valid
 * @param errorMsg
 * @return
 */
PmuppCollection PParamDataHandler::ReadDbFile(const QString fln, bool &valid, QString &errorMsg)
{
  PmuppCollection collection;
  PmuppRun run;
  QFile file(fln);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    errorMsg = QString("couldn't open ") + fln;
    cerr << endl;
    cerr << "----" << endl;
    cerr << "**ERROR** " << errorMsg.toLatin1().data() << endl;
    cerr << "----" << endl;
    valid = false;
    return collection;
  }

  QTextStream in(&file);
  QString line;
  QStringList token, subTok;
  bool done = false, param_found = false, ok;
  int ival;
  double dval;
  PmuppParam param;
  run.SetName(fln);
  while (!in.atEnd() && !done) {
    line = in.readLine();
    // ignore title, abstract, labels and data-tags for now
    if (line.startsWith("\\-e")) {
      param_found = true;
      line = in.readLine();
    }
    if (param_found && !line.isEmpty()) {
      // check if parameter or run number and title
      token.clear();
      token = line.split(",", QString::SkipEmptyParts);
      if (token.size()==0) {
        errorMsg = fln + QString(". No parameter tokens.");
        cerr << endl;
        cerr << "----" << endl;
        cerr << "**ERROR** in " << errorMsg.toLatin1().data() << endl;
        cerr << "----" << endl;
        file.close();
        valid = false;
        return collection;
      }
      ival = token[0].toInt(&ok);
      if (ok) { // run,,, title found
        run.SetNumber(ival);
        QString title("");
        for (int i=1; i<token.size(); i++)
          title += token[i];
        title = title.trimmed();
        run.SetName(title);
        // check that the number of parameters is the same for all runs
        if (collection.GetNoOfRuns() > 0) {
          if (collection.GetRun(0).GetNoOfParam() != run.GetNoOfParam()) {
            errorMsg = fln + QString(".\n");
            errorMsg += QString("  first   run (#%1) has %2 params.\n").arg(collection.GetRun(0).GetNumber()).arg(collection.GetRun(0).GetNoOfParam());
            errorMsg += QString("  current run (#%1) has %2 params.\n").arg(run.GetNumber()).arg(run.GetNoOfParam());
            errorMsg += QString("  Inspect your db-file!");
            cerr << endl;
            cerr << "----" << endl;
            cerr << "**ERROR** in " << errorMsg.toLatin1().data() << endl;
            cerr << "----" << endl;
            file.close();
            valid = false;
            return collection;
          }
        }
        collection.AddRun(run);
        run.Clear();
      } else { // parameter
        if (token.size() != 4) { // wrong number of parameter tokens
          cerr << endl;
          cerr << "----" << endl;
          cerr << "**ERROR** in" << fln.toLatin1().data() <<". # parameter tokens != 4." << endl;
          cerr << "----" << endl;
          file.close();
          valid = false;
          return collection;
        }
        subTok = token[0].split("=");
        if (subTok.size() != 2) {
          cerr << endl;
          cerr << "----" << endl;
          cerr << "**ERROR** in" << fln.toLatin1().data() <<". parameter name=value token missing." << endl;
          cerr << "----" << endl;
          file.close();
          valid = false;
          return collection;
        }
        QString paramName = subTok[0].trimmed();
        param.ResetParam();
        param.SetName(paramName);
        dval = subTok[1].toDouble(&ok);
        if (ok) {
          param.SetValue(dval);
        } else {
          cerr << endl;
          cerr << "----" << endl;
          cerr << "**ERROR** in" << fln.toLatin1().data() <<". parameter name=value token missing or wrong?!" << endl;
          cerr << "----" << endl;
          file.close();
          valid = false;
          return collection;
        }
        // pos. err
        dval = token[1].toDouble(&ok);
        if (ok) {
          param.SetPosErr(dval);
        } else {
          cerr << endl;
          cerr << "----" << endl;
          cerr << "**ERROR** in" << fln.toLatin1().data() <<". parameter pos. error not a number?!" << endl;
          cerr << "----" << endl;
          file.close();
          valid = false;
          return collection;
        }
        // neg. err
        dval = token[2].toDouble(&ok);
        if (ok) {
          param.SetNegErr(dval);
        } else {
          cerr << endl;
          cerr << "----" << endl;
          cerr << "**ERROR** in" << fln.toLatin1().data() <<". parameter neg. error not a number?!" << endl;
          cerr << "----" << endl;
          file.close();
          valid = false;
          return collection;
        }
        run.AddParam(param);
      }
    }
  }
  if (!param_found) {
    cerr << endl;
    cerr << "----" << endl;
    cerr << "**ERROR** in" << fln.toLatin1().data() <<". No parameter found." << endl;
    cerr << "----" << endl;
  }
  file.close();

  valid = true;

  return collection;
}

//--------------------------------------------------------------------------
/**
 * @brief PParamDataHandler::ReadColumnParamFile
 * @param fln
 * @param valid
 * @param errorMsg
 * @return
 */
PmuppCollection PParamDataHandler::ReadColumnParamFile(const QString fln, bool &valid, QString &errorMsg)
{
  PmuppCollection collection;
  PmuppRun run;
  valid = true;

  QFile file(fln);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    errorMsg = QString("couldn't open ")+fln;
    cerr << endl;
    cerr << "----" << endl;
    cerr << "**ERROR** " << errorMsg.toLatin1().data() << endl;
    cerr << "----" << endl;
    valid = false;
    return collection;
  }

  QTextStream in(&file);
  QString line;
  QStringList token;
  bool done = false, ok;
  double dval = 0;
  int ival;
  PmuppParam param;
  run.SetName(fln);

  // read header information
  line = in.readLine();
  token = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);

  QVector<QString> headerInfo;
  QVector<int> headerCode; // 0=value, 1=pos/neg err, 2=pos err, 3=neg err, 4=run number
  int code=PMUPP_UNDEF;
  for (int i=0; i<token.size(); i++) {
    headerInfo.push_back(token[i]);
    if (token[i].contains("PosErr"))
      code = PMUPP_POSERR;
    else if (token[i].contains("NegErr"))
      code = PMUPP_NEGERR;
    else if (token[i].contains("Err"))
      code = PMUPP_POSNEGERR;
    else if (token[i].contains("RUN"))
      code = PMUPP_RUN;
    else
      code = PMUPP_VALUE;
    headerCode.push_back(code);
  }

  // parameters
  int lineNo = 1;
  while (!in.atEnd() && !done) {
    line = in.readLine();
    if (line.isEmpty())
      continue;
    lineNo++;
    token.clear();
    token = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
    // paranoia check
    if (token.size() != headerInfo.size()) {
      errorMsg = QString("size mismatch between header and parameter int line: %1 (header=%2 / param=%3)").arg(lineNo).arg(headerInfo.size()).arg(token.size());
      cerr << endl;
      cerr << "----" << endl;
      cerr << "**ERROR** " << errorMsg.toLatin1().data() << endl;
      cerr << "----" << endl;
      valid = false;
      file.close();
      return collection;
    }
    for (int i=0; i<token.size(); i++) {
      if (headerCode[i] == PMUPP_RUN)
        ival = token[i].toInt(&ok);
      else
        dval = token[i].toDouble(&ok);
      if (!ok) {
        errorMsg = QString("unrecognized token ('%1') in line %2 (line number: %3)").arg(token[i].toLatin1().data()).arg(line.toLatin1().data()).arg(lineNo);
        cerr << endl;
        cerr << "----" << endl;
        cerr << "**ERROR** " << errorMsg.toLatin1().data() << endl;
        cerr << "----" << endl;
        valid = false;
        file.close();
        return collection;
      }

      if (headerCode[i] == PMUPP_VALUE) {
        param.SetName(headerInfo[i]);
        param.SetValue(dval);
        if (i+1 < token.size()) {
          if (headerCode[i+1] == PMUPP_VALUE) {
            param.SetNegErr(0.0);
            param.SetPosErr(0.0);
            run.AddParam(param);
            param.ResetParam();
          }
        }
      } else if (headerCode[i] == PMUPP_POSNEGERR) {
        param.SetPosErr(dval);
        param.SetNegErr(dval);
        run.AddParam(param);
        param.ResetParam();
      } else if (headerCode[i] == PMUPP_POSERR) {
        param.SetPosErr(dval);
      } else if (headerCode[i] == PMUPP_NEGERR) {
        param.SetNegErr(dval);
        run.AddParam(param);
        param.ResetParam();
      } else if (headerCode[i] == PMUPP_RUN) {
        param.ResetParam();
        run.SetNumber(ival);
        run.SetName(QString("%1").arg(ival));
        QString collName("");
        int pos = fln.lastIndexOf("/");
        if (pos == -1)
          collName = fln;
        else
          collName = fln.right(fln.length()-pos-1);
        collection.SetName(collName);
        collection.AddRun(run);
        run.Clear();
      }

    }
  }
  file.close();

  return collection;
}

//--------------------------------------------------------------------------
/**
 * @brief PParamDataHandler::GetCollection
 * @param idx
 * @param valid
 * @return
 */
PmuppCollection PParamDataHandler::GetCollection(const int idx, bool &valid)
{
  valid = true;
  PmuppCollection collection;

  if ((idx<0) || (idx>=fCollection.size())) {
    valid = false;
    return collection;
  }

  return fCollection[idx];
}

//--------------------------------------------------------------------------
/**
 * @brief PParamDataHandler::GetCollection
 * @param idx
 * @return
 */
PmuppCollection *PParamDataHandler::GetCollection(const int idx)
{
  if ((idx<0) || (idx>=fCollection.size())) {
    return 0;
  }

  return &fCollection[idx];
}

//--------------------------------------------------------------------------
/**
 * @brief PParamDataHandler::GetCollection
 * @param name
 * @param valid
 * @return
 */
PmuppCollection PParamDataHandler::GetCollection(const QString name, bool &valid)
{
  valid = true;
  PmuppCollection collection;

  int idx=-1;
  for (int i=0; i<fCollection.size(); i++) {
    if (!fCollection[i].GetName().compare(name)) {
      idx = i;
      break;
    }
  }

  if (idx==-1) { // not found
    valid = false;
    return collection;
  }

  return fCollection[idx];
}

//--------------------------------------------------------------------------
/**
 * @brief PParamDataHandler::GetCollection
 * @param name
 * @return
 */
PmuppCollection *PParamDataHandler::GetCollection(const QString name)
{
  int idx=-1;
  for (int i=0; i<fCollection.size(); i++) {
    if (!fCollection[i].GetName().compare(name)) {
      idx = i;
      break;
    }
  }

  if (idx==-1) { // not found
    return 0;
  }

  return &fCollection[idx];
}

//--------------------------------------------------------------------------
/**
 * @brief PParamDataHandler::GetCollectionIndex
 * @param name
 * @return
 */
int PParamDataHandler::GetCollectionIndex(const QString name)
{
  int idx=-1;

  for (int i=0; i<fCollection.size(); i++) {
    if (!fCollection[i].GetName().compare(name)) {
      idx = i;
      break;
    }
  }

  return idx;
}

//--------------------------------------------------------------------------
/**
 * @brief PParamDataHandler::GetCollectionName
 * @param idx
 * @return
 */
QString PParamDataHandler::GetCollectionName(const int idx)
{
  QString name = QString("??");

  if ((idx >= 0) && (idx < fCollection.size()))
    name = fCollection[idx].GetName();

  return name;
}

//--------------------------------------------------------------------------
/**
 * @brief PParamDataHandler::GetValues
 * @param collName
 * @param paramName
 * @return
 */
QVector<double> PParamDataHandler::GetValues(QString collName, QString paramName)
{
  QVector<double> data;

  // find collection with correct name
  int idx=-1;
  for (int i=0; i<fCollection.size(); i++) {
    if (fCollection[i].GetName() == collName) {
      idx = i;
      break;
    }
  }
  if (idx == -1) { // collection not found
    return data;
  }

  // make sure parameter name is present in collection
  PmuppRun run = fCollection[idx].GetRun(0);
  int idx1 = -1;
  for (int i=0; i<run.GetNoOfParam(); i++) {
    if (run.GetParam(i).GetName() == paramName) {
      idx1 = i;
      break;
    }
  }
  if (idx1 == -1) { // parameter name not found
    return data;
  }

  for (int i=0; i<fCollection[idx].GetNoOfRuns(); i++) {
    data.push_back(fCollection[idx].GetRun(i).GetParam(idx1).GetValue());
  }

  return data;
}

//--------------------------------------------------------------------------
/**
 * @brief PParamDataHandler::GetPosErr
 * @param collName
 * @param paramName
 * @return
 */
QVector<double> PParamDataHandler::GetPosErr(QString collName, QString paramName)
{
  QVector<double> data;

  // find collection with correct name
  int idx=-1;
  for (int i=0; i<fCollection.size(); i++) {
    if (fCollection[i].GetName() == collName) {
      idx = i;
      break;
    }
  }
  if (idx == -1) { // collection not found
    return data;
  }

  // make sure parameter name is present in collection
  PmuppRun run = fCollection[idx].GetRun(0);
  int idx1 = -1;
  for (int i=0; i<run.GetNoOfParam(); i++) {
    if (run.GetParam(i).GetName() == paramName) {
      idx1 = i;
      break;
    }
  }
  if (idx1 == -1) { // parameter name not found
    return data;
  }

  for (int i=0; i<fCollection[idx].GetNoOfRuns(); i++) {
    data.push_back(fCollection[idx].GetRun(i).GetParam(idx1).GetPosErr());
  }

  return data;
}

//--------------------------------------------------------------------------
/**
 * @brief PParamDataHandler::GetNegErr
 * @param collName
 * @param paramName
 * @return
 */
QVector<double> PParamDataHandler::GetNegErr(QString collName, QString paramName)
{
  QVector<double> data;

  // find collection with correct name
  int idx=-1;
  for (int i=0; i<fCollection.size(); i++) {
    if (fCollection[i].GetName() == collName) {
      idx = i;
      break;
    }
  }
  if (idx == -1) { // collection not found
    return data;
  }

  // make sure parameter name is present in collection
  PmuppRun run = fCollection[idx].GetRun(0);
  int idx1 = -1;
  for (int i=0; i<run.GetNoOfParam(); i++) {
    if (run.GetParam(i).GetName() == paramName) {
      idx1 = i;
      break;
    }
  }
  if (idx1 == -1) { // parameter name not found
    return data;
  }

  for (int i=0; i<fCollection[idx].GetNoOfRuns(); i++) {
    data.push_back(fCollection[idx].GetRun(i).GetParam(idx1).GetNegErr());
  }

  return data;
}

//--------------------------------------------------------------------------
/**
 * @brief PParamDataHandler::RemoveCollection
 * @param name
 */
void PParamDataHandler::RemoveCollection(QString name)
{
  for (int i=0; i<fCollection.size(); i++) {
    if (fCollection[i].GetName() == name) {
      fCollection.remove(i);
      break;
    }
  }
}

//--------------------------------------------------------------------------
/**
 * @brief PParamDataHandler::ReplaceCollection
 * @param coll
 * @param idx
 */
void PParamDataHandler::ReplaceCollection(PmuppCollection coll, int idx)
{
  if (idx >= fCollection.size())
    return;

  fCollection.replace(idx, coll);
}

//--------------------------------------------------------------------------
/**
 * @brief PParamDataHandler::Dump
 */
void PParamDataHandler::Dump()
{
  PmuppRun run;
  PmuppParam param;

  for (int i=0; i<fCollection.size(); i++) {
    cout << "debug> +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-" << endl;
    cout << "debug> collection name: " << fCollection[i].GetName().toLatin1().data() << endl;
    cout << "debug> +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-" << endl;
    for (int j=0; j<fCollection[i].GetNoOfRuns(); j++) {
      cout << "debug> ----------------------------------" << endl;
      run = fCollection[i].GetRun(j);
      cout << "debug>> run number: " << run.GetNumber() << endl;
      cout << "debug>> run name  : " << run.GetName().toLatin1().data() << endl;
      cout << "debug>> ---------------------------------" << endl;
      for (int k=0; k<run.GetNoOfParam(); k++) {
        param = run.GetParam(k);
        cout << "debug>>> " << param.GetName().toLatin1().data() << ": " << param.GetValue() << " -+ " << param.GetNegErr() << " / " << param.GetPosErr() << endl;
      }
    }
  }
}

//--------------------------------------------------------------------------
/**
 * @brief PParamDataHandler::readFromStdOut
 */
void PParamDataHandler::readFromStdOut()
{
  qInfo() << fProc->readAllStandardOutput();
}

//--------------------------------------------------------------------------
/**
 * @brief PParamDataHandler::readFromStdErr
 */
void PParamDataHandler::readFromStdErr()
{
  qInfo() << fProc->readAllStandardError();
}

//--------------------------------------------------------------------------
/**
 * @brief PParamDataHandler::processDone
 * @param exitCode
 * @param exitStatus
 */
void PParamDataHandler::processDone(int exitCode, QProcess::ExitStatus exitStatus)
{
  qInfo() << "in processDone()";
  if ((exitStatus == QProcess::CrashExit) && (exitCode != 0))
    qInfo() << "**ERROR** processDone: exitCode = " << exitCode << endl;
}
