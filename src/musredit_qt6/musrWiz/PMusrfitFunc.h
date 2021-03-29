/***************************************************************************

  PMusrfitFunc.h

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007-2021 by Andreas Suter                              *
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

#ifndef _PMUSRFITFUNC_H_
#define _PMUSRFITFUNC_H_

#include <QString>
#include <QVector>

//---------------------------------------------------------------------------
class PFuncParam {
  public:
    PFuncParam() { initParam(); }
    ~PFuncParam() {}

    void initParam() { fName = "UnDef"; fValue = 0.0; fMap = false; }

    void setParamName(QString name) { fName = name; }
    void setParamValue(double dval) { fValue = dval; }
    void setParamMap(bool isMap) { fMap = isMap; }

    QString getParamName() { return fName; }
    double getParamValue() { return fValue; }
    bool isMap() { return fMap; }

  private:
    QString fName;
    double fValue;
    bool fMap;
};

//---------------------------------------------------------------------------
class PMusrfitFunc {
  public:
    PMusrfitFunc() { initFunc(); }
    ~PMusrfitFunc() {}

    void initFunc();

    void setName(QString name) { fName = name; }
    void setAbbrv(QString abbrv) { fAbbrv = abbrv; }
    void setNoOfParam(int ival) { fNoOfParam = ival; }
    void addFuncParam(PFuncParam param) { fParam.push_back(param); }

    QString getName() { return fName; }
    QString getAbbrv() { return fAbbrv; }
    int getNoOfParam() { return fNoOfParam; }
    PFuncParam getFuncParam(int idx);
    QVector<PFuncParam> *getFuncParams() { return &fParam; }

  private:
    QString fName;
    QString fAbbrv;
    int fNoOfParam;
    QVector<PFuncParam> fParam;
};

#endif // _PMUSRFITFUNC_H_
