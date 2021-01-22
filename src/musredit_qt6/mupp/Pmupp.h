/***************************************************************************

  Pmupp.h

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

#ifndef _PMUPP_H_
#define _PMUPP_H_

#include <QObject>
#include <QString>
#include <QVector>
#include <QStringList>
#include <QProcess>

#define MUPP_UNDEF 1.0e99

//----------------------------------------------------------------------------
/**
 * <p>The PmuppParam class. It handles a single parameter: name, value,
 * positive- and negative error.
 */
class PmuppParam {
  public:
    PmuppParam();
    PmuppParam(QString name, double param, double posErr);
    PmuppParam(QString name, double param, double posErr, double negErr);

    void ResetParam();
    void SetParam(QString name, double param, double posErr);
    void SetParam(QString name, double param, double posErr, double negErr);
    void SetName(QString name) { fName = name; }
    void SetValue(double dval) { fValue = dval; }
    void SetPosErr(double dval) { fPosErr = dval; }
    void SetNegErr(double dval) { fNegErr = dval; }

    QString GetName() { return fName; }
    double GetValue() { return fValue; }
    double GetPosErr() { return fPosErr; }
    double GetNegErr() { return fNegErr; }

  private:
    QString fName; ///< parameter name
    double fValue; ///< parameter value
    double fPosErr; ///< positive error of the parameter
    double fNegErr; ///< negative error of the parameter
};

//----------------------------------------------------------------------------
/**
 * <p>The PmuppRun class. Contains all the parameters of a single run.
 */
class PmuppRun {
  public:
    PmuppRun() { fNumber = -1; fName=""; fParam.clear(); }
    ~PmuppRun() { fNumber = -1; fName=""; fParam.clear(); }

    void Clear() { fNumber = -1; fName=""; fParam.clear(); }
    void SetName(QString name) { fName = name; }
    void SetNumber(int ival) { fNumber = ival; }
    void AddParam(PmuppParam param) { fParam.push_back(param); }

    int GetNumber() { return fNumber; }
    QString GetName() { return fName; }
    int GetNoOfParam() { return fParam.size(); }
    PmuppParam GetParam(unsigned int idx);

  private:
    int fNumber; ///< run number
    QString fName; ///< name of the run
    QVector<PmuppParam> fParam; ///< parameters of the run
};

//----------------------------------------------------------------------------
/**
 * <p>The PmuppCollection class. Is collecting a number of runs. Typically
 * something like a full temperature scan, an energy scan, etc.
 */
class PmuppCollection {
  public:
    PmuppCollection() { fPathName=""; fName = ""; fRun.clear(); }

    void SetPathName(QString pathName) { fPathName = pathName; }
    void SetName (QString name) { fName = name; }
    void AddRun(PmuppRun run) { fRun.push_back(run); }

    QString GetPathName() { return fPathName; }
    QString GetName() { return fName; }
    int GetNoOfRuns() { return fRun.size(); }
    PmuppRun GetRun(unsigned int idx);

  private:
    QString fPathName; ///< path-name of the collection
    QString fName; ///< name of the collection
    QVector<PmuppRun> fRun; ///< all the runs in the collection
};

//----------------------------------------------------------------------------
/**
 * <p>The PParamDataHandler class. This class handles all the collections
 * loaded.
 */
class PParamDataHandler : public QObject {
  Q_OBJECT

  public:
    PParamDataHandler() {}

    int GetNoOfCollections() { return fCollection.size(); }
    void NewCollection(const QString name);

    bool ReadParamFile(const QStringList fln, QString &errorMsg);
    PmuppCollection ReadDbFile(const QString fln, bool &valid, QString &errorMsg);
    PmuppCollection ReadColumnParamFile(const QString fln, bool &valid, QString &errorMsg);

    PmuppCollection GetCollection(const int idx, bool &valid);
    PmuppCollection GetCollection(const QString name, bool &valid);
    PmuppCollection *GetCollection(const int idx);
    PmuppCollection *GetCollection(const QString name);
    int GetCollectionIndex(const QString name);
    QString GetCollectionName(const int idx);

    QVector<double> GetValues(QString collName, QString paramName);
    QVector<double> GetPosErr(QString collName, QString paramName);
    QVector<double> GetNegErr(QString collName, QString paramName);

    void RemoveCollection(QString name);
    void ReplaceCollection(PmuppCollection coll, int idx);

    void Dump();

  signals:
    void newData();

  private:
    QProcess *fProc; ///< this will be needed if msr2data needs to be called
    QVector<PmuppCollection> fCollection; ///< all the collections handeled

    bool analyzeFileList(const QStringList &fln, QString &collectionName,
                         QStringList &arg, QString &workDir, QString &errorMsg);

  private slots:
    void readFromStdOut();
    void readFromStdErr();
    void processDone(int, QProcess::ExitStatus);
};

#endif // _PMUPP_H_
