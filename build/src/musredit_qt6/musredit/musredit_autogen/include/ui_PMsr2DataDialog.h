/********************************************************************************
** Form generated from reading UI file 'PMsr2DataDialog.ui'
**
** Created by: Qt User Interface Compiler version 6.4.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PMSR2DATADIALOG_H
#define UI_PMSR2DATADIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PMsr2DataDialog
{
public:
    QGroupBox *fRunListInput_groupBox;
    QWidget *layoutWidget_0;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout_3;
    QLabel *fRunList_label;
    QLineEdit *fRunList_lineEdit;
    QSpacerItem *horizontalSpacer_4;
    QLabel *fOrRunList_label;
    QHBoxLayout *horizontalLayout_4;
    QLabel *fRunListFileName_label;
    QLineEdit *fRunListFileName_lineEdit;
    QSpacerItem *horizontalSpacer_5;
    QGroupBox *fMsrFileExtension_groupBox;
    QWidget *layoutWidget_1;
    QHBoxLayout *horizontalLayout_5;
    QLabel *fExtension_label;
    QSpacerItem *horizontalSpacer_6;
    QLineEdit *fMsrFileExtension_lineEdit;
    QSpacerItem *horizontalSpacer_7;
    QGroupBox *fTemplateRunInput_groupBox;
    QWidget *layoutWidget_2;
    QHBoxLayout *horizontalLayout_6;
    QLabel *fTemplateRunNumber_label;
    QLineEdit *fTemplateRunNumber_lineEdit;
    QCheckBox *fChainFit_checkBox;
    QSpacerItem *horizontalSpacer_8;
    QGroupBox *fDataOutputFileName_groupBox;
    QWidget *layoutWidget_3;
    QHBoxLayout *horizontalLayout_7;
    QLabel *fDataOutputFileName_label;
    QLineEdit *fDataOutputFileName_lineEdit;
    QSpacerItem *horizontalSpacer_9;
    QGroupBox *fOptions_groupBox;
    QWidget *layoutWidget_4;
    QHBoxLayout *horizontalLayout_9;
    QVBoxLayout *verticalLayout_3;
    QCheckBox *fWriteDataHeader_checkBox;
    QCheckBox *fIgnoreDataHeaderInfo_checkBox;
    QCheckBox *fKeepMinuit2Output_checkBox;
    QCheckBox *fEstimateN0_checkBox;
    QVBoxLayout *verticalLayout_4;
    QCheckBox *fWriteColumnData_checkBox;
    QCheckBox *fRecreateDataFile_checkBox;
    QCheckBox *fOpenFilesAfterFitting_checkBox;
    QCheckBox *fWritePerRunBlockChisq_checkBox;
    QVBoxLayout *verticalLayout_5;
    QCheckBox *fTitleFromData_checkBox;
    QCheckBox *fCreateMsrFileOnly_checkBox;
    QCheckBox *fFitOnly_checkBox;
    QHBoxLayout *horizontalLayout_8;
    QCheckBox *fGlobal_checkBox;
    QCheckBox *fGlobalPlus_checkBox;
    QLabel *fParamList_label;
    QLineEdit *fParamList_lineEdit;
    QWidget *layoutWidget_5;
    QVBoxLayout *verticalLayout;
    QFrame *line;
    QHBoxLayout *horizontalLayout;
    QPushButton *fHelp_pushButton;
    QSpacerItem *horizontalSpacer;
    QPushButton *fOk_pushButton;
    QPushButton *fCancel_pushButton;

    void setupUi(QDialog *PMsr2DataDialog)
    {
        if (PMsr2DataDialog->objectName().isEmpty())
            PMsr2DataDialog->setObjectName("PMsr2DataDialog");
        PMsr2DataDialog->setWindowModality(Qt::WindowModal);
        PMsr2DataDialog->resize(585, 594);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/images/musrmsr2data.xpm"), QSize(), QIcon::Normal, QIcon::Off);
        PMsr2DataDialog->setWindowIcon(icon);
        fRunListInput_groupBox = new QGroupBox(PMsr2DataDialog);
        fRunListInput_groupBox->setObjectName("fRunListInput_groupBox");
        fRunListInput_groupBox->setGeometry(QRect(0, 0, 581, 111));
        QFont font;
        font.setBold(false);
        fRunListInput_groupBox->setFont(font);
        fRunListInput_groupBox->setFlat(false);
        fRunListInput_groupBox->setCheckable(false);
        layoutWidget_0 = new QWidget(fRunListInput_groupBox);
        layoutWidget_0->setObjectName("layoutWidget_0");
        layoutWidget_0->setGeometry(QRect(10, 30, 561, 70));
        verticalLayout_2 = new QVBoxLayout(layoutWidget_0);
        verticalLayout_2->setObjectName("verticalLayout_2");
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        fRunList_label = new QLabel(layoutWidget_0);
        fRunList_label->setObjectName("fRunList_label");

        horizontalLayout_3->addWidget(fRunList_label);

        fRunList_lineEdit = new QLineEdit(layoutWidget_0);
        fRunList_lineEdit->setObjectName("fRunList_lineEdit");

        horizontalLayout_3->addWidget(fRunList_lineEdit);

        horizontalSpacer_4 = new QSpacerItem(28, 20, QSizePolicy::Minimum, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_4);

        fOrRunList_label = new QLabel(layoutWidget_0);
        fOrRunList_label->setObjectName("fOrRunList_label");
        fOrRunList_label->setTextFormat(Qt::AutoText);

        horizontalLayout_3->addWidget(fOrRunList_label);


        verticalLayout_2->addLayout(horizontalLayout_3);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName("horizontalLayout_4");
        fRunListFileName_label = new QLabel(layoutWidget_0);
        fRunListFileName_label->setObjectName("fRunListFileName_label");

        horizontalLayout_4->addWidget(fRunListFileName_label);

        fRunListFileName_lineEdit = new QLineEdit(layoutWidget_0);
        fRunListFileName_lineEdit->setObjectName("fRunListFileName_lineEdit");

        horizontalLayout_4->addWidget(fRunListFileName_lineEdit);

        horizontalSpacer_5 = new QSpacerItem(52, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_5);


        verticalLayout_2->addLayout(horizontalLayout_4);

        fMsrFileExtension_groupBox = new QGroupBox(PMsr2DataDialog);
        fMsrFileExtension_groupBox->setObjectName("fMsrFileExtension_groupBox");
        fMsrFileExtension_groupBox->setGeometry(QRect(0, 110, 581, 71));
        layoutWidget_1 = new QWidget(fMsrFileExtension_groupBox);
        layoutWidget_1->setObjectName("layoutWidget_1");
        layoutWidget_1->setGeometry(QRect(10, 30, 561, 31));
        horizontalLayout_5 = new QHBoxLayout(layoutWidget_1);
        horizontalLayout_5->setObjectName("horizontalLayout_5");
        horizontalLayout_5->setContentsMargins(0, 0, 0, 0);
        fExtension_label = new QLabel(layoutWidget_1);
        fExtension_label->setObjectName("fExtension_label");

        horizontalLayout_5->addWidget(fExtension_label);

        horizontalSpacer_6 = new QSpacerItem(30, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_6);

        fMsrFileExtension_lineEdit = new QLineEdit(layoutWidget_1);
        fMsrFileExtension_lineEdit->setObjectName("fMsrFileExtension_lineEdit");

        horizontalLayout_5->addWidget(fMsrFileExtension_lineEdit);

        horizontalSpacer_7 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_7);

        fTemplateRunInput_groupBox = new QGroupBox(PMsr2DataDialog);
        fTemplateRunInput_groupBox->setObjectName("fTemplateRunInput_groupBox");
        fTemplateRunInput_groupBox->setGeometry(QRect(0, 180, 581, 80));
        layoutWidget_2 = new QWidget(fTemplateRunInput_groupBox);
        layoutWidget_2->setObjectName("layoutWidget_2");
        layoutWidget_2->setGeometry(QRect(10, 30, 561, 31));
        horizontalLayout_6 = new QHBoxLayout(layoutWidget_2);
        horizontalLayout_6->setObjectName("horizontalLayout_6");
        horizontalLayout_6->setContentsMargins(0, 0, 0, 0);
        fTemplateRunNumber_label = new QLabel(layoutWidget_2);
        fTemplateRunNumber_label->setObjectName("fTemplateRunNumber_label");

        horizontalLayout_6->addWidget(fTemplateRunNumber_label);

        fTemplateRunNumber_lineEdit = new QLineEdit(layoutWidget_2);
        fTemplateRunNumber_lineEdit->setObjectName("fTemplateRunNumber_lineEdit");

        horizontalLayout_6->addWidget(fTemplateRunNumber_lineEdit);

        fChainFit_checkBox = new QCheckBox(layoutWidget_2);
        fChainFit_checkBox->setObjectName("fChainFit_checkBox");

        horizontalLayout_6->addWidget(fChainFit_checkBox);

        horizontalSpacer_8 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_6->addItem(horizontalSpacer_8);

        fDataOutputFileName_groupBox = new QGroupBox(PMsr2DataDialog);
        fDataOutputFileName_groupBox->setObjectName("fDataOutputFileName_groupBox");
        fDataOutputFileName_groupBox->setGeometry(QRect(0, 260, 581, 80));
        layoutWidget_3 = new QWidget(fDataOutputFileName_groupBox);
        layoutWidget_3->setObjectName("layoutWidget_3");
        layoutWidget_3->setGeometry(QRect(10, 30, 561, 31));
        horizontalLayout_7 = new QHBoxLayout(layoutWidget_3);
        horizontalLayout_7->setObjectName("horizontalLayout_7");
        horizontalLayout_7->setContentsMargins(0, 0, 0, 0);
        fDataOutputFileName_label = new QLabel(layoutWidget_3);
        fDataOutputFileName_label->setObjectName("fDataOutputFileName_label");

        horizontalLayout_7->addWidget(fDataOutputFileName_label);

        fDataOutputFileName_lineEdit = new QLineEdit(layoutWidget_3);
        fDataOutputFileName_lineEdit->setObjectName("fDataOutputFileName_lineEdit");

        horizontalLayout_7->addWidget(fDataOutputFileName_lineEdit);

        horizontalSpacer_9 = new QSpacerItem(40, 20, QSizePolicy::Preferred, QSizePolicy::Minimum);

        horizontalLayout_7->addItem(horizontalSpacer_9);

        fOptions_groupBox = new QGroupBox(PMsr2DataDialog);
        fOptions_groupBox->setObjectName("fOptions_groupBox");
        fOptions_groupBox->setGeometry(QRect(0, 340, 581, 191));
        layoutWidget_4 = new QWidget(fOptions_groupBox);
        layoutWidget_4->setObjectName("layoutWidget_4");
        layoutWidget_4->setGeometry(QRect(6, 20, 560, 131));
        horizontalLayout_9 = new QHBoxLayout(layoutWidget_4);
        horizontalLayout_9->setObjectName("horizontalLayout_9");
        horizontalLayout_9->setContentsMargins(0, 0, 0, 0);
        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName("verticalLayout_3");
        fWriteDataHeader_checkBox = new QCheckBox(layoutWidget_4);
        fWriteDataHeader_checkBox->setObjectName("fWriteDataHeader_checkBox");

        verticalLayout_3->addWidget(fWriteDataHeader_checkBox);

        fIgnoreDataHeaderInfo_checkBox = new QCheckBox(layoutWidget_4);
        fIgnoreDataHeaderInfo_checkBox->setObjectName("fIgnoreDataHeaderInfo_checkBox");

        verticalLayout_3->addWidget(fIgnoreDataHeaderInfo_checkBox);

        fKeepMinuit2Output_checkBox = new QCheckBox(layoutWidget_4);
        fKeepMinuit2Output_checkBox->setObjectName("fKeepMinuit2Output_checkBox");

        verticalLayout_3->addWidget(fKeepMinuit2Output_checkBox);

        fEstimateN0_checkBox = new QCheckBox(layoutWidget_4);
        fEstimateN0_checkBox->setObjectName("fEstimateN0_checkBox");

        verticalLayout_3->addWidget(fEstimateN0_checkBox);


        horizontalLayout_9->addLayout(verticalLayout_3);

        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setObjectName("verticalLayout_4");
        fWriteColumnData_checkBox = new QCheckBox(layoutWidget_4);
        fWriteColumnData_checkBox->setObjectName("fWriteColumnData_checkBox");

        verticalLayout_4->addWidget(fWriteColumnData_checkBox);

        fRecreateDataFile_checkBox = new QCheckBox(layoutWidget_4);
        fRecreateDataFile_checkBox->setObjectName("fRecreateDataFile_checkBox");

        verticalLayout_4->addWidget(fRecreateDataFile_checkBox);

        fOpenFilesAfterFitting_checkBox = new QCheckBox(layoutWidget_4);
        fOpenFilesAfterFitting_checkBox->setObjectName("fOpenFilesAfterFitting_checkBox");

        verticalLayout_4->addWidget(fOpenFilesAfterFitting_checkBox);

        fWritePerRunBlockChisq_checkBox = new QCheckBox(layoutWidget_4);
        fWritePerRunBlockChisq_checkBox->setObjectName("fWritePerRunBlockChisq_checkBox");

        verticalLayout_4->addWidget(fWritePerRunBlockChisq_checkBox);


        horizontalLayout_9->addLayout(verticalLayout_4);

        verticalLayout_5 = new QVBoxLayout();
        verticalLayout_5->setObjectName("verticalLayout_5");
        fTitleFromData_checkBox = new QCheckBox(layoutWidget_4);
        fTitleFromData_checkBox->setObjectName("fTitleFromData_checkBox");

        verticalLayout_5->addWidget(fTitleFromData_checkBox);

        fCreateMsrFileOnly_checkBox = new QCheckBox(layoutWidget_4);
        fCreateMsrFileOnly_checkBox->setObjectName("fCreateMsrFileOnly_checkBox");

        verticalLayout_5->addWidget(fCreateMsrFileOnly_checkBox);

        fFitOnly_checkBox = new QCheckBox(layoutWidget_4);
        fFitOnly_checkBox->setObjectName("fFitOnly_checkBox");

        verticalLayout_5->addWidget(fFitOnly_checkBox);

        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setObjectName("horizontalLayout_8");
        fGlobal_checkBox = new QCheckBox(layoutWidget_4);
        fGlobal_checkBox->setObjectName("fGlobal_checkBox");

        horizontalLayout_8->addWidget(fGlobal_checkBox);

        fGlobalPlus_checkBox = new QCheckBox(layoutWidget_4);
        fGlobalPlus_checkBox->setObjectName("fGlobalPlus_checkBox");

        horizontalLayout_8->addWidget(fGlobalPlus_checkBox);


        verticalLayout_5->addLayout(horizontalLayout_8);


        horizontalLayout_9->addLayout(verticalLayout_5);

        fParamList_label = new QLabel(fOptions_groupBox);
        fParamList_label->setObjectName("fParamList_label");
        fParamList_label->setGeometry(QRect(9, 160, 141, 16));
        fParamList_lineEdit = new QLineEdit(fOptions_groupBox);
        fParamList_lineEdit->setObjectName("fParamList_lineEdit");
        fParamList_lineEdit->setGeometry(QRect(164, 157, 401, 23));
        layoutWidget_5 = new QWidget(PMsr2DataDialog);
        layoutWidget_5->setObjectName("layoutWidget_5");
        layoutWidget_5->setGeometry(QRect(5, 540, 571, 51));
        verticalLayout = new QVBoxLayout(layoutWidget_5);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        line = new QFrame(layoutWidget_5);
        line->setObjectName("line");
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        verticalLayout->addWidget(line);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        fHelp_pushButton = new QPushButton(layoutWidget_5);
        fHelp_pushButton->setObjectName("fHelp_pushButton");

        horizontalLayout->addWidget(fHelp_pushButton);

        horizontalSpacer = new QSpacerItem(178, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        fOk_pushButton = new QPushButton(layoutWidget_5);
        fOk_pushButton->setObjectName("fOk_pushButton");

        horizontalLayout->addWidget(fOk_pushButton);

        fCancel_pushButton = new QPushButton(layoutWidget_5);
        fCancel_pushButton->setObjectName("fCancel_pushButton");

        horizontalLayout->addWidget(fCancel_pushButton);


        verticalLayout->addLayout(horizontalLayout);

        QWidget::setTabOrder(fRunList_lineEdit, fRunListFileName_lineEdit);
        QWidget::setTabOrder(fRunListFileName_lineEdit, fMsrFileExtension_lineEdit);
        QWidget::setTabOrder(fMsrFileExtension_lineEdit, fTemplateRunNumber_lineEdit);
        QWidget::setTabOrder(fTemplateRunNumber_lineEdit, fChainFit_checkBox);
        QWidget::setTabOrder(fChainFit_checkBox, fDataOutputFileName_lineEdit);
        QWidget::setTabOrder(fDataOutputFileName_lineEdit, fWriteDataHeader_checkBox);
        QWidget::setTabOrder(fWriteDataHeader_checkBox, fIgnoreDataHeaderInfo_checkBox);
        QWidget::setTabOrder(fIgnoreDataHeaderInfo_checkBox, fKeepMinuit2Output_checkBox);
        QWidget::setTabOrder(fKeepMinuit2Output_checkBox, fWriteColumnData_checkBox);
        QWidget::setTabOrder(fWriteColumnData_checkBox, fRecreateDataFile_checkBox);
        QWidget::setTabOrder(fRecreateDataFile_checkBox, fOpenFilesAfterFitting_checkBox);
        QWidget::setTabOrder(fOpenFilesAfterFitting_checkBox, fTitleFromData_checkBox);
        QWidget::setTabOrder(fTitleFromData_checkBox, fCreateMsrFileOnly_checkBox);
        QWidget::setTabOrder(fCreateMsrFileOnly_checkBox, fFitOnly_checkBox);
        QWidget::setTabOrder(fFitOnly_checkBox, fGlobal_checkBox);
        QWidget::setTabOrder(fGlobal_checkBox, fGlobalPlus_checkBox);
        QWidget::setTabOrder(fGlobalPlus_checkBox, fParamList_lineEdit);
        QWidget::setTabOrder(fParamList_lineEdit, fCancel_pushButton);
        QWidget::setTabOrder(fCancel_pushButton, fHelp_pushButton);
        QWidget::setTabOrder(fHelp_pushButton, fOk_pushButton);

        retranslateUi(PMsr2DataDialog);
        QObject::connect(fCancel_pushButton, &QPushButton::clicked, PMsr2DataDialog, qOverload<>(&QDialog::reject));
        QObject::connect(fOk_pushButton, &QPushButton::clicked, PMsr2DataDialog, qOverload<>(&QDialog::accept));
        QObject::connect(fHelp_pushButton, SIGNAL(clicked()), PMsr2DataDialog, SLOT(helpContent()));
        QObject::connect(fRunList_lineEdit, SIGNAL(textChanged(QString)), PMsr2DataDialog, SLOT(runListEntered(QString)));
        QObject::connect(fRunListFileName_lineEdit, SIGNAL(textChanged(QString)), PMsr2DataDialog, SLOT(runListFileNameEntered(QString)));
        QObject::connect(fTemplateRunNumber_lineEdit, SIGNAL(textChanged(QString)), PMsr2DataDialog, SLOT(templateRunEntered(QString)));
        QObject::connect(fFitOnly_checkBox, SIGNAL(stateChanged(int)), PMsr2DataDialog, SLOT(fitOnlyChanged(int)));
        QObject::connect(fCreateMsrFileOnly_checkBox, SIGNAL(stateChanged(int)), PMsr2DataDialog, SLOT(createMsrFileOnlyChanged(int)));

        QMetaObject::connectSlotsByName(PMsr2DataDialog);
    } // setupUi

    void retranslateUi(QDialog *PMsr2DataDialog)
    {
        PMsr2DataDialog->setWindowTitle(QCoreApplication::translate("PMsr2DataDialog", "msr2data input", nullptr));
        fRunListInput_groupBox->setTitle(QCoreApplication::translate("PMsr2DataDialog", "Run List Input", nullptr));
        fRunList_label->setText(QCoreApplication::translate("PMsr2DataDialog", "Run List", nullptr));
#if QT_CONFIG(whatsthis)
        fRunList_lineEdit->setWhatsThis(QCoreApplication::translate("PMsr2DataDialog", "<html><head/><body><p>The run list consists of a collection of run number. Accepted input formats are:</p><p>(i) &lt;run0&gt; &lt;run1&gt; ... &lt;runN&gt;, e.g. 124 126 129</p><p>(ii) &lt;run0&gt;-&lt;runN&gt;, e.g. 124-126, i.e. 124 125 126</p><p>(iii) &lt;run0&gt;:&lt;runN&gt;:&lt;step&gt;, e.g 124:128:2, i.e. 124 126 128</p><p>or combination of those three.</p></body></html>", nullptr));
#endif // QT_CONFIG(whatsthis)
        fOrRunList_label->setText(QCoreApplication::translate("PMsr2DataDialog", "<b>OR</b>", nullptr));
        fRunListFileName_label->setText(QCoreApplication::translate("PMsr2DataDialog", "Run List File Name", nullptr));
#if QT_CONFIG(whatsthis)
        fRunListFileName_lineEdit->setWhatsThis(QCoreApplication::translate("PMsr2DataDialog", "<html><head/><body><p>an ASCII file containing a list of run numbers and optional external parameters is passed to msr2data. </p><p>For details see the online documentation.</p></body></html>", nullptr));
#endif // QT_CONFIG(whatsthis)
        fMsrFileExtension_groupBox->setTitle(QCoreApplication::translate("PMsr2DataDialog", "msr File Extension", nullptr));
        fExtension_label->setText(QCoreApplication::translate("PMsr2DataDialog", "Extension", nullptr));
#if QT_CONFIG(whatsthis)
        fMsrFileExtension_lineEdit->setWhatsThis(QCoreApplication::translate("PMsr2DataDialog", "<html><head/><body><p>the extension will be used together with the run number to generate the msr-file name. For example: the run number being 123 and the extension _tf_h13, an msr-file name 123_tf_h13.msr will result.</p></body></html>", nullptr));
#endif // QT_CONFIG(whatsthis)
        fTemplateRunInput_groupBox->setTitle(QCoreApplication::translate("PMsr2DataDialog", "Template Run Input", nullptr));
        fTemplateRunNumber_label->setText(QCoreApplication::translate("PMsr2DataDialog", "Template Run Number", nullptr));
#if QT_CONFIG(whatsthis)
        fTemplateRunNumber_lineEdit->setWhatsThis(QCoreApplication::translate("PMsr2DataDialog", "<html><head/><body><p>the run number given here will be used as a msr-file template number to generate/fit the run's given in the 'Run List Input'.</p></body></html>", nullptr));
#endif // QT_CONFIG(whatsthis)
#if QT_CONFIG(whatsthis)
        fChainFit_checkBox->setWhatsThis(QCoreApplication::translate("PMsr2DataDialog", "<html><head/><body><p>unselected means: all msr-files generated and fitted will start from the given template.</p><p>selected means: the msr-files generated and fitted will use the previously fitted msr-file as an input. This makes sense if the run list given has continously changing parameters, e.g. as function of the temperature.</p></body></html>", nullptr));
#endif // QT_CONFIG(whatsthis)
        fChainFit_checkBox->setText(QCoreApplication::translate("PMsr2DataDialog", "Chain Fit", nullptr));
        fDataOutputFileName_groupBox->setTitle(QCoreApplication::translate("PMsr2DataDialog", "Parameter Output File Name", nullptr));
        fDataOutputFileName_label->setText(QCoreApplication::translate("PMsr2DataDialog", "Parameters Output File Name", nullptr));
#if QT_CONFIG(whatsthis)
        fDataOutputFileName_lineEdit->setWhatsThis(QCoreApplication::translate("PMsr2DataDialog", "<html><head/><body><p>db- or dat-output file name for the parameter files.</p></body></html>", nullptr));
#endif // QT_CONFIG(whatsthis)
        fOptions_groupBox->setTitle(QCoreApplication::translate("PMsr2DataDialog", "Options", nullptr));
#if QT_CONFIG(whatsthis)
        fWriteDataHeader_checkBox->setWhatsThis(QCoreApplication::translate("PMsr2DataDialog", "<html><head/><body><p>For db-files, a Data Header will be written.</p></body></html>", nullptr));
#endif // QT_CONFIG(whatsthis)
        fWriteDataHeader_checkBox->setText(QCoreApplication::translate("PMsr2DataDialog", "Write Data Header", nullptr));
#if QT_CONFIG(whatsthis)
        fIgnoreDataHeaderInfo_checkBox->setWhatsThis(QCoreApplication::translate("PMsr2DataDialog", "<html><head/><body><p>This tag is used in conjunction with LEM. If <span style=\" font-weight:600;\">not</span> selected, it will try to extract experiment specific parameters from the data file like implantation energy, transport HV settings, etc.</p></body></html>", nullptr));
#endif // QT_CONFIG(whatsthis)
        fIgnoreDataHeaderInfo_checkBox->setText(QCoreApplication::translate("PMsr2DataDialog", "Ignore Data Header Info", nullptr));
#if QT_CONFIG(whatsthis)
        fKeepMinuit2Output_checkBox->setWhatsThis(QCoreApplication::translate("PMsr2DataDialog", "<html><head/><body><p>selected: for each run fitted, two additional files will be written, namely a &lt;msr-filename&gt;-mn2.output and &lt;msr-filename&gt;-mn2.root, which contain a richer set of information about fit, i.e. the covariance matrix, the correlation coefficients, etc.</p></body></html>", nullptr));
#endif // QT_CONFIG(whatsthis)
        fKeepMinuit2Output_checkBox->setText(QCoreApplication::translate("PMsr2DataDialog", "Keep Minuit2 Output", nullptr));
        fEstimateN0_checkBox->setText(QCoreApplication::translate("PMsr2DataDialog", "Estimate N0", nullptr));
#if QT_CONFIG(whatsthis)
        fWriteColumnData_checkBox->setWhatsThis(QCoreApplication::translate("PMsr2DataDialog", "<html><head/><body><p>unselected: the output parameter file is written in so called db-format.</p><p>selected: the output parameter file is written in column like ascii-format.</p></body></html>", nullptr));
#endif // QT_CONFIG(whatsthis)
        fWriteColumnData_checkBox->setText(QCoreApplication::translate("PMsr2DataDialog", "Write Column Data", nullptr));
#if QT_CONFIG(whatsthis)
        fRecreateDataFile_checkBox->setWhatsThis(QCoreApplication::translate("PMsr2DataDialog", "<html><head/><body><p>unselected: if the parameter output file already exists, the parameters will be appended.</p><p>selected: if the parameter output file already exists: it will be deleted and a new one will be written.</p></body></html>", nullptr));
#endif // QT_CONFIG(whatsthis)
        fRecreateDataFile_checkBox->setText(QCoreApplication::translate("PMsr2DataDialog", "Recreate Data File", nullptr));
#if QT_CONFIG(whatsthis)
        fOpenFilesAfterFitting_checkBox->setWhatsThis(QCoreApplication::translate("PMsr2DataDialog", "<html><head/><body><p>selected: the newly generated msr-files will be opened in musredit after the fit took place.</p></body></html>", nullptr));
#endif // QT_CONFIG(whatsthis)
        fOpenFilesAfterFitting_checkBox->setText(QCoreApplication::translate("PMsr2DataDialog", "Open Files after Fitting", nullptr));
        fWritePerRunBlockChisq_checkBox->setText(QCoreApplication::translate("PMsr2DataDialog", "Write per-run-block Chisq", nullptr));
#if QT_CONFIG(whatsthis)
        fTitleFromData_checkBox->setWhatsThis(QCoreApplication::translate("PMsr2DataDialog", "<html><head/><body><p>if selected: the run title of the generated msr-file will be the one given in the muSR data file.</p></body></html>", nullptr));
#endif // QT_CONFIG(whatsthis)
        fTitleFromData_checkBox->setText(QCoreApplication::translate("PMsr2DataDialog", "Take Data File Title", nullptr));
#if QT_CONFIG(whatsthis)
        fCreateMsrFileOnly_checkBox->setWhatsThis(QCoreApplication::translate("PMsr2DataDialog", "<html><head/><body><p>if selected: the msr-files will be created, but <span style=\" font-weight:600;\">no </span>fitting will take place.</p></body></html>", nullptr));
#endif // QT_CONFIG(whatsthis)
        fCreateMsrFileOnly_checkBox->setText(QCoreApplication::translate("PMsr2DataDialog", "Create msr-File only", nullptr));
#if QT_CONFIG(whatsthis)
        fFitOnly_checkBox->setWhatsThis(QCoreApplication::translate("PMsr2DataDialog", "<html><head/><body><p>selected: it is assumed that the msr-files already exist, and only musrfit is called.</p></body></html>", nullptr));
#endif // QT_CONFIG(whatsthis)
        fFitOnly_checkBox->setText(QCoreApplication::translate("PMsr2DataDialog", "Fit Only", nullptr));
#if QT_CONFIG(whatsthis)
        fGlobal_checkBox->setWhatsThis(QCoreApplication::translate("PMsr2DataDialog", "<html><head/><body><p>selected: will generate a msr-file for a global fit. Please check the online documentation for further details.</p></body></html>", nullptr));
#endif // QT_CONFIG(whatsthis)
        fGlobal_checkBox->setText(QCoreApplication::translate("PMsr2DataDialog", "Global", nullptr));
#if QT_CONFIG(whatsthis)
        fGlobalPlus_checkBox->setWhatsThis(QCoreApplication::translate("PMsr2DataDialog", "<html><head/><body><p>selected: will generate a msr-file for a global fit. The difference between Global and Global+ is that for Global+ the input parameters of the msr-file are originating from the single run fits. Please check the online documentation for further details.</p></body></html>", nullptr));
#endif // QT_CONFIG(whatsthis)
        fGlobalPlus_checkBox->setText(QCoreApplication::translate("PMsr2DataDialog", "Global+", nullptr));
#if QT_CONFIG(whatsthis)
        fParamList_label->setWhatsThis(QCoreApplication::translate("PMsr2DataDialog", "<html><head/><body><p>parameter numbers to be exported, e.g. 1-16, or 1 3-7, etc.</p></body></html>", nullptr));
#endif // QT_CONFIG(whatsthis)
        fParamList_label->setText(QCoreApplication::translate("PMsr2DataDialog", "Parameter Export List", nullptr));
#if QT_CONFIG(whatsthis)
        fParamList_lineEdit->setWhatsThis(QCoreApplication::translate("PMsr2DataDialog", "<html><head/><body><p>parameter numbers to be exported, e.g. 1-16, or 1 3-7, etc.</p></body></html>", nullptr));
#endif // QT_CONFIG(whatsthis)
        fHelp_pushButton->setText(QCoreApplication::translate("PMsr2DataDialog", "&Help", nullptr));
        fOk_pushButton->setText(QCoreApplication::translate("PMsr2DataDialog", "&OK", nullptr));
        fCancel_pushButton->setText(QCoreApplication::translate("PMsr2DataDialog", "&Cancel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class PMsr2DataDialog: public Ui_PMsr2DataDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PMSR2DATADIALOG_H
