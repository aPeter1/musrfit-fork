/***************************************************************************

  PTheoTemplate.h

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

#ifndef _PTHEOTEMPLATE_H_
#define _PTHEOTEMPLATE_H_

#include <QString>
#include <QVector>

#include "musrWiz.h"

class PTheoTemplate {
  public:
    PTheoTemplate() { init(); }
    ~PTheoTemplate() {}

    void init();

    QString getName() { return fPreDefName; }
    QString getTheory() { return fTheory; }
    int getNoOfFunc() { return fFunc.size(); }
    QString getFunc(int idx);
    int getFuncNo(int idx);
    int getNoOfMap() { return fMap.size(); }
    PParam getMap(int idx);
    int getNoOfParam() { return fParam.size(); }
    PParam getParam(int idx);

    void setName(QString str) { fPreDefName = str; }
    void setTheory(QString str) { fTheory = str; }
    void appendFunc(QString str);
    void appendParam(PParam param);
    void appendMap(PParam map);

  private:
    QString fPreDefName;
    QString fTheory;
    QVector<PParam> fParam;
    QVector<QString> fFunc;
    QVector<PParam> fMap;
};

#endif // _PTHEOTEMPLATE_H_
