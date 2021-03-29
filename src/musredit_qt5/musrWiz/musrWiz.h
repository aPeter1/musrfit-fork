/***************************************************************************

  musrWiz.h

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

#ifndef _MUSRWIZ_H_
#define _MUSRWIZ_H_

#include <QString>

//-------------------------------------------------------------------
class PParam {
  public:
    PParam() { init(); }
    ~PParam() {}

    void init();

    QString getName() { return fName; }
    int getNumber() { return fNumber; }
    double getValue() { return fValue; }
    double getStep() { return fStep; }
    QString getPosErr() { return fPosErr; }
    QString getBoundLow() { return fBoundLow; }
    QString getBoundHigh() { return fBoundHigh; }

    void setName(QString str) { fName = str; }
    void setNumber(int ival) { fNumber = ival; }
    void setValue(double dval) { fValue = dval; }
    void setStep(double dval) { fStep = dval; }
    void setPosErr(QString str) { fPosErr = str; }
    void setBoundLow(QString str) { fBoundLow = str; }
    void setBoundHigh(QString str) { fBoundHigh = str; }

  private:
    QString fName;
    int fNumber;
    double fValue;
    double fStep;
    QString fPosErr;
    QString fBoundLow;
    QString fBoundHigh;
};

//-------------------------------------------------------------------
typedef struct {
  int number;
  QString fun;
} PFunc;

#endif // _MUSRWIZ_H_
