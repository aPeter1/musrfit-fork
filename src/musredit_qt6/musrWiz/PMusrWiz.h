/***************************************************************************

  PMusrWiz.h

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

#ifndef _PMUSRWIZ_
#define _PMUSRWIZ_

#include <QString>
#include <QStringList>
#include <QWizard>
#include <QWizardPage>
#include <QObject>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QFormLayout>

#include "musrWiz.h"
#include "PAdmin.h"

#define INST_UNDEF  0
#define INST_PSI    1
#define INST_TRIUMF 2
#define INST_ISIS   3
#define INST_JPARC  4

#define FIT_TYPE_UNDEF            0
#define FIT_TYPE_SINGLE_HISTO     1
#define FIT_TYPE_SINGLE_HISTO_RRF 2
#define FIT_TYPE_ASYMMETRY        3
#define FIT_TYPE_ASYMMETRY_RRF    4
#define FIT_TYPE_MU_MINUS         5
#define FIT_TYPE_NONE_MUSR        6

#define MEASURE_UNDEF 0
#define MEASURE_ZF    1
#define MEASURE_TF    2
#define MEASURE_LF    3

#define T0_FROM_FILE    0
#define T0_FROM_MUSR_T0 1
#define T0_ENTER_WIZ    2

//-------------------------------------------------------------------
struct PParamGui {
  QLineEdit *paramName;
  QLineEdit *paramVal;
  QLineEdit *paramStep;
  QLineEdit *paramBoundLow;
  QLineEdit *paramBoundHigh;
};

//-------------------------------------------------------------------
class PMsrData
{
  public:
    PMsrData();
    ~PMsrData();

    void clearParam() { fParam.clear(); }
    void clearFunc() { fFunc.clear(); }
    void clearMap() { fMap.clear(); }

    void setMsrPathName(QString str) { fMsrPathName = str; }
    void setMsrFileName(QString fileName) { fMsrFileName = fileName; }
    void appendMsrFileName(QString str) { fMsrFileName += str; }
    void setYear(QString year) { fYear = year; }
    void setRunNumber(int runNo) { fRunNumber = runNo; }
    void setInstitute(QString institute) { fInstitute = institute; }
    void setInstrument(QString instrument) { fInstrument = instrument; }
    void setSetup(QString setup) { fSetup = setup; }
    void setFitType(int fitType) { fFitType = fitType; }
    void setTypeOfMeasurement(int type) { fTypeOfMeasurement = type; }
    void setT0Tag(int tag) { fT0Tag = tag; }
    void setT0(int t0) { fT0 = t0; }
    void setTheory(QString str) { fTheo = str; }
    void setParam(QVector<PParam> param) { fParam = param; }
    void setParam(int idx, PParam param);
    void appendParam(QVector<PParam> param);
    void setMap(QVector<PParam> map) { fMap = map; }
    void setMap(int idx, PParam map);
    void appendMap(QVector<PParam> map);
    void setMapTemplateName(int idx, QString name);
    void setFunc(int funNo, QString str="??");
    void setPacking(int pack) { fPacking = abs(pack); }
    void setFitStart(double start) { fFitStart = start; }
    void setFitEnd(double end) { fFitEnd = end; }
    void setCmd(QString cmd) { fCommands = cmd; }
    void setTemplate(int templ) { fTemplate = templ; }
    void setTemplate(PTheoTemplate templ) { fTheoTempl = templ; }

    QString getMsrPathName() { return fMsrPathName; }
    QString getMsrFileName() { return fMsrFileName; }
    QString getYear() { return fYear; }
    int getRunNumber() { return fRunNumber; }
    QString getInstitute() { return fInstitute; }
    QString getInstrument() { return fInstrument; }
    QString getSetup() { return fSetup; }
    int getFitType() { return fFitType; }
    QString getFitTypeString();
    int getTypeOfMeasurement() { return fTypeOfMeasurement; }
    int getT0Tag() { return fT0Tag; }
    int getT0() { return fT0; }
    QString getTheory() { return fTheo; }
    int getNoOfParam() { return fParam.size(); }
    PParam getParam(int idx);
    int getNoOfMap() { return fMap.size(); }
    PParam getMap(int idx);
    int getNoOfFunc() { return fFunc.size(); }
    int getFuncNo(int idx);
    bool isPresent(const QString funStr);
    QString getFunc(int funNo);
    QString getFuncAll();
    int getPacking() { return fPacking; }
    double getFitStart() { return fFitStart; }
    double getFitEnd() { return fFitEnd; }
    QString getCmd() { return fCommands; }
    bool isTemplate() { return (fTemplate == -1) ? false : true; }
    PTheoTemplate getTemplate() { return fTheoTempl; }

    void sort(QString whichVec);
    void removeFunc(QVector<int> &funList);

  private:
    int fTemplate;
    QString fMsrPathName;
    QString fMsrFileName;
    int fRunNumber;
    QString fInstitute;
    QString fInstrument;
    QString fSetup;
    int fFitType;
    int fTypeOfMeasurement;
    int fT0Tag;
    int fT0;
    QString fYear;
    int fPacking;
    double fFitStart;
    double fFitEnd;
    QString fCommands;

    QString fTheo;

    PTheoTemplate   fTheoTempl;
    QVector<PParam> fParam;
    QVector<PFunc>  fFunc;
    QVector<PParam> fMap;
};

//-------------------------------------------------------------------
class PShowTheo : public QDialog
{
  public:
    PShowTheo(QString theo, QString func = QString(""),
              QWidget *parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
};

//-------------------------------------------------------------------
class PIntroPage : public QWizardPage
{
  Q_OBJECT

  public:
    PIntroPage(PAdmin *admin, PMsrData *data, QWidget *parent=0);

    int nextId() const Q_DECL_OVERRIDE;
    bool validatePage() override;

  private slots:
    void handleInstituteChanged(int idx);
    void handleFitType(int idx);
    void checkSetup(int idx);
    void handleT0(int idx);

  private:
    PAdmin *fAdmin;
    PMsrData *fMsrData;

    QLineEdit *fMsrFileName;
    QLineEdit *fYear;
    QLineEdit *fRunNumber;
    QComboBox *fInstitute;
    QComboBox *fInstrument;
    QComboBox *fFitType;
    QComboBox *fMeasurementType;
    QComboBox *fT0;
};

//-------------------------------------------------------------------
class PTheoPage : public QWizardPage
{
  Q_OBJECT

  public:
    PTheoPage(PAdmin *admin, PMsrData *data, QWidget *parent=0);

    int nextId() const Q_DECL_OVERRIDE;
    void initializePage() override;
    bool validatePage() override;

  private slots:
    void templateState(int);
    void clearAll();
    void addTheory();
    void checkTheory();

  private:
    PAdmin *fAdmin;
    PMsrData *fMsrData;

    bool fTheoValid;
    bool fHaveMap;
    bool fHaveFunc;

    int fTheoBlockNo;

    QVector<PParam> fParamList;
    QVector<PParam> fMapList;
    QVector<int> fFunList;

    QPlainTextEdit *fTheo;
    QCheckBox *fEditTemplate;
    QPushButton *fClearAll;
    QComboBox *fTheoSelect;
    QPushButton *fTheoAdd;
    QPushButton *fCheckTheo;

    QString getTheoryFunction(int idx);
    bool analyzeTokens(QString str, int noOfTokens);
    void dealWithParam(QString theo, int paramNo, int paramIdx);
    void dealWithMap(QString theo, int mapNo, int paramIdx);
    void dealWithFun(int funNo);
};

//-------------------------------------------------------------------
class PFuncPage : public QWizardPage
{
  Q_OBJECT

  public:
    PFuncPage(PMsrData *data, QWidget *parent=0);

    int nextId() const Q_DECL_OVERRIDE;
    void initializePage() override;
    void cleanupPage() override;
    bool validatePage() override;

  private slots:
    void showTheo();

  private:
    PMsrData *fMsrData;

    QFormLayout *fMapPageLayout;
    QPlainTextEdit *fFunc;
    QPushButton *fShowTheo;

    int getFuncNo(const QString str);
};

//-------------------------------------------------------------------
class PMapPage : public QWizardPage
{
  Q_OBJECT

  public:
    PMapPage(PMsrData *data, QWidget *parent=0);

    int nextId() const Q_DECL_OVERRIDE;
    void initializePage() override;
    bool validatePage() override;

  private slots:
    void showTheo();

  private:
    PMsrData *fMsrData;

    QFormLayout *fMapPageLayout;
    QVector<PParamGui> fMapGuiVec;
    QPushButton *fShowTheo;
};

//-------------------------------------------------------------------
class PParamPage : public QWizardPage
{
  Q_OBJECT

  public:
    PParamPage(PMsrData *data, QWidget *parent=0);

    int nextId() const Q_DECL_OVERRIDE;
    void initializePage() override;
    bool validatePage() override;

  private slots:
    void showTheo();

  private:
    PMsrData *fMsrData;

    QFormLayout *fParameterPageLayout;
    QVector<PParamGui> fParamGuiVec;
    QPushButton *fShowTheo;
};

//-------------------------------------------------------------------
class PFitInfoPage : public QWizardPage
{
  Q_OBJECT

  public:
    PFitInfoPage(PMsrData *data, QWidget *parent=0);

    int nextId() const Q_DECL_OVERRIDE;
    void initializePage() override;
    bool validatePage() override;

  private:
    PMsrData *fMsrData;

    QLineEdit *fFitRangeStart;
    QLineEdit *fFitRangeEnd;
    QLineEdit *fPacking;
    QPlainTextEdit *fCommands;
};

//-------------------------------------------------------------------
class PConclusionPage : public QWizardPage
{
  Q_OBJECT

  public:
    PConclusionPage(PAdmin *admin, PMsrData *data, QString *msrFilePath, QWidget *parent=0);

  private slots:
    void saveAsMsrFile();
    void saveAsTemplate();

  private:
    QString *fMsrFilePath;
    PAdmin *fAdmin;
    PMsrData *fMsrData;

    QLabel *fMsrPathFileLabel;
    QLineEdit *fMsrFilePathLineEdit;
    QPushButton *fSaveAsMsrFile;
    QPushButton *fSaveAsTemplate;
};

//-------------------------------------------------------------------
class PMusrWiz : public QWizard
{
  Q_OBJECT

  friend class PIntroPage;
  friend class PTheoPage;
  friend class PMapPage;
  friend class PFuncPage;
  friend class PParamPage;
  friend class PFitInfoPage;

  public:
    PMusrWiz(QWidget *parent=Q_NULLPTR);
    PMusrWiz(PAdmin *admin, PMsrData *msrData, QWidget *parent=Q_NULLPTR);
    virtual ~PMusrWiz();

  private slots:
    virtual int writeMsrFile(int result);
    virtual void help();

  protected:
    enum {ePageIntro, ePageTheory, ePageMaps, ePageFunctions, ePageParameters, ePageFitInfo, ePageConclusion};

  private:
    QString fMsrFilePath;
    PAdmin *fAdmin;
    PMsrData *fMsrData;

    QString getRunName(PInstrument *instru);

    virtual int writeMsrFileSingleHisto();
    virtual int writeMsrFileAsymmetry();
};

#endif //  _PMUSRWIZ_
