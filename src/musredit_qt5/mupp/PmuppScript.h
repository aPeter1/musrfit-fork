/***************************************************************************

  PmuppScript.h

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

#ifndef _PMUPPSCRIPT_H_
#define _PMUPPSCRIPT_H_

#include <QString>
#include <QStringList>

#include "PmuppAdmin.h"
#include "Pmupp.h"
#include "PVarHandler.h"

//----------------------------------------------------------------------------
typedef struct {
  int collIdx; ///< collection index
  QString xLabel; ///< x-axis label
  QVector<QString> yLabel; ///< y-axis label(s)
} PmuppPlotEntry;

//----------------------------------------------------------------------------
/**
 * @brief The PmuppScript class. mupp script class which is used as a scripting
 * interface for mupp.
 */
class PmuppScript : public QObject
{
  Q_OBJECT

  public:
    PmuppScript(QStringList script);
    ~PmuppScript();

    void setLoadPath(const QString cmd);
    QString getLoadPath() { return fLoadPath; }

    void setSavePath(const QString cmd);
    QString getSavePath() { return fSavePath; }

    int loadCollection(const QString str);
    int select(const QString str);
    int selectAll();
    int addX(const QString str);
    int addY(const QString str);
    int plot(const QString str);
    int macro(const QString str, const QString plotFln="");
    int var_cmd(const QString str);

  public slots:
    int executeScript();

  signals:
    void finished();

  private:
    PmuppAdmin *fAdmin; ///< admin object

    QStringList fScript; ///< script source
    PParamDataHandler *fParamDataHandler; ///< parameter data handler
    int fSelected; ///< -2=nothing selected, -1=all selected, >=0 is the index if the selected collection

    PmuppPlotEntry fPlotEntry; ///< plot entry object
    QVector<PmuppPlotEntry> fPlotInfo; ///< vector of all plot entry objects

    bool fNorm; ///< normalization flag, true -> normalize y-data before plotting.
    QString fLoadPath; ///< load path specifying where to look for data.
    QString fSavePath; ///< save path specifying where to save data.

    QVector<PVarHandler> fVarHandler; ///< variable handler vector

    bool foundLabel(PmuppCollection *coll, const QString label);
    bool foundVariable(const QString var);
    int  getVarIndex(const QString var);
    void minMax(QVector<double> dvec, double &min, double &max);
    QString getNicerLabel(const QString label);

    int getCollectionIndex(const QString var_name);
};

#endif // _PMUPPSCRIPT_H_
