/***************************************************************************

  PInstrumentDef.h

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

#ifndef _PINSTRUMENTDEF_H_
#define _PINSTRUMENTDEF_H_

#include <QString>
#include <QVector>

//---------------------------------------------------------------------------
class PDetector
{
  public:
    PDetector() {}
    ~PDetector() {}

    void setName(QString str) { fName = str; }
    void setRelGeomPhase(double phase) { fRelGeomPhase = phase; }
    void setForwards(QVector<int> num) { fForward = num; }
    void setBackwards(QVector<int> num) { fBackward = num; }
    void setAlpha(double alpha) { fAlpha = alpha; }

    QString getName() { return fName; }
    double getRelGeomPhase() { return fRelGeomPhase; }
    QVector<int> getForwards() { return fForward; }
    QVector<int> getBackwards() { return fBackward; }
    double getAlpha() { return fAlpha; }

  private:
    QString fName;
    QVector<int> fForward;
    QVector<int> fBackward;
    double fAlpha;
    double  fRelGeomPhase;
};

//---------------------------------------------------------------------------
class PSetup
{
  public:
    PSetup();
    ~PSetup() {}

    void setName(QString str) { fName = str; }
    void setNoOfDetectors(int no) { fNoOfDetectors = no; }
    void setFgbOffset(int fgbOffset) { fFgbOffset = fgbOffset; }
    void setLgb(int lgb) { fLgb = lgb; }
    void setBkgRange(int start, int end) { fBkgStartBin = start; fBkgEndBin = end; }
    void addDetector(PDetector detector) { fLogicDetectors.push_back(detector); }
    void addAsymDetector(PDetector detector) { fLogicAsymDetectors.push_back(detector); }

    QString getName() { return fName; }
    int getNoOfDetectors() { return fNoOfDetectors; }
    int getNoOfLogicalDetectors() { return fLogicDetectors.size(); }
    int getNoOfLogicalAsymDetectors() { return fLogicAsymDetectors.size(); }
    int getFgbOffset() { return fFgbOffset; }
    int getLgb() { return fLgb; }
    int getBkgStartBin() { return fBkgStartBin; }
    int getBkgEndBin() { return fBkgEndBin; }
    PDetector* getDetector(int idx);
    PDetector* getAsymDetector(int idx);

  private:
    QString fName;
    int fNoOfDetectors;
    int fFgbOffset;
    int fLgb;
    int fBkgStartBin;
    int fBkgEndBin;

    QVector<PDetector> fLogicDetectors;
    QVector<PDetector> fLogicAsymDetectors;
};

//---------------------------------------------------------------------------
class PInstrument
{
  public:
    PInstrument();
    ~PInstrument() {}

    void setRunNameTemplate(QString str) { fRunNameTemplate = str; }
    void setBeamline(QString str) { fBeamline = str; }
    void setDataFileFormat(QString str) { fDataFileFormat = str; }
    void setInstitue(QString str) { fInstitue = str; }
    void setName(QString str) { fName = str; }

    void addSetupZF(PSetup zf) { fZF.push_back(zf); }
    void addSetupTF(PSetup tf) { fTF.push_back(tf); }
    void addSetupLF(PSetup lf) { fLF.push_back(lf); }

    QString getRunNameTemplate() { return fRunNameTemplate; }
    QString getBeamline() { return fBeamline; }
    QString getDataFileFormat() { return fDataFileFormat; }
    QString getInstitute() { return fInstitue; }
    QString getName() { return fName; }

    QVector<PSetup>  getZFSetups() { return fZF; }
    QVector<PSetup>  getTFSetups() { return fTF; }
    QVector<PSetup>  getLFSetups() { return fLF; }

    PSetup* getZFSetup(QString name="");
    PSetup* getTFSetup(QString name="");
    PSetup* getLFSetup(QString name="");

  private:
    QString fRunNameTemplate;
    QString fBeamline;
    QString fDataFileFormat;
    QString fInstitue;
    QString fName;

    QVector<PSetup> fZF;
    QVector<PSetup> fTF;
    QVector<PSetup> fLF;
};

//---------------------------------------------------------------------------
class PInstitute
{
  public:
    PInstitute();
    ~PInstitute() {}

    void setName(QString str) { fName = str; }
    void addInstrument(PInstrument instrument) { fInstrument.push_back(instrument); }

    QString getName() { return fName; }
    QVector<PInstrument> getInstruments() { return fInstrument; }
    PInstrument *getInstrument(QString name);

  private:
    QString fName;
    QVector<PInstrument> fInstrument;
};

#endif // _PINSTRUMENTDEF_H_
