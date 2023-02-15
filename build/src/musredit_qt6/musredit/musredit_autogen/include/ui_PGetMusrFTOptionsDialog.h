/********************************************************************************
** Form generated from reading UI file 'PGetMusrFTOptionsDialog.ui'
**
** Created by: Qt User Interface Compiler version 6.4.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PGETMUSRFTOPTIONSDIALOG_H
#define UI_PGETMUSRFTOPTIONSDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PGetMusrFTOptionsDialog
{
public:
    QGroupBox *fFourier_groupBox;
    QWidget *widget;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout_22;
    QHBoxLayout *horizontalLayout_13;
    QLabel *fFourierOptions_label;
    QComboBox *fFourierOption_comboBox;
    QSpacerItem *horizontalSpacer_6;
    QHBoxLayout *horizontalLayout_14;
    QLabel *fFourierUnits_label;
    QComboBox *fFourierUnits_comboBox;
    QHBoxLayout *horizontalLayout_23;
    QHBoxLayout *horizontalLayout_19;
    QLabel *fFourierPower_label;
    QLineEdit *fFourierPower_lineEdit;
    QSpacerItem *horizontalSpacer_5;
    QHBoxLayout *horizontalLayout_17;
    QLabel *fLifetimeCorrection_label;
    QLineEdit *fLifetimeCorrection_lineEdit;
    QHBoxLayout *horizontalLayout_20;
    QLabel *fFourierRange_label;
    QHBoxLayout *horizontalLayout_16;
    QLabel *fFourierRangeStart_label;
    QLineEdit *fFourierRangeStart_lineEdit;
    QHBoxLayout *horizontalLayout_15;
    QLabel *fFourierRangeEnd_label;
    QLineEdit *fFourierRangeEnd_lineEdit;
    QSpacerItem *horizontalSpacer_3;
    QHBoxLayout *horizontalLayout_18;
    QCheckBox *fAveragedView_checkBox;
    QCheckBox *fAveragePerDataSet_checkBox;
    QSpacerItem *horizontalSpacer_7;
    QCheckBox *fCreateMsrFile_checkBox;
    QHBoxLayout *horizontalLayout_21;
    QLabel *fFourierTitle_label;
    QLineEdit *fFourierTitle_lineEdit;
    QGroupBox *fHistoInfo_groupBox;
    QWidget *layoutWidget_0;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_3;
    QLabel *fBkgRange_label;
    QHBoxLayout *horizontalLayout;
    QLabel *fBkgStartBin_label;
    QLineEdit *fBkgRangeStartBin_lineEdit;
    QHBoxLayout *horizontalLayout_2;
    QLabel *fBkgEndBin_label;
    QLineEdit *fBkgRangeEndBin_lineEdit;
    QHBoxLayout *horizontalLayout_4;
    QLabel *fBkgList_label;
    QLineEdit *fBkgList_lineEdit;
    QHBoxLayout *horizontalLayout_7;
    QHBoxLayout *horizontalLayout_6;
    QLabel *fApodization_label;
    QComboBox *fApodization_comboBox;
    QHBoxLayout *horizontalLayout_5;
    QLabel *fPacking_label;
    QLineEdit *fPacking_lineEdit;
    QSpacerItem *horizontalSpacer;
    QHBoxLayout *horizontalLayout_10;
    QLabel *fTimeRange_label;
    QSpacerItem *horizontalSpacer_2;
    QHBoxLayout *horizontalLayout_9;
    QLabel *fTimeRangeStart_label;
    QLineEdit *fTimeRangeStart_lineEdit;
    QHBoxLayout *horizontalLayout_8;
    QLabel *fTimeRangeEnd_label;
    QLineEdit *fTimeRangeEnd_lineEdit;
    QHBoxLayout *horizontalLayout_11;
    QLabel *fHistoList_label;
    QLineEdit *fHistoList_lineEdit;
    QHBoxLayout *horizontalLayout_12;
    QLabel *fT0_label;
    QLineEdit *fT0_lineEdit;
    QGroupBox *fFileSelection_groupBox;
    QWidget *layoutWidget_1;
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *horizontalLayout_27;
    QCheckBox *fCurrentMsrFile_checkBox;
    QCheckBox *fAllMsrFiles_checkBox;
    QSpacerItem *horizontalSpacer_8;
    QHBoxLayout *horizontalLayout_25;
    QPushButton *fMsrFileSelector_pushButton;
    QLineEdit *fMsrFileSelector_lineEdit;
    QPushButton *fMsrFileNameClear_pushButton;
    QHBoxLayout *horizontalLayout_26;
    QPushButton *fDataFileSelector_pushButton;
    QLineEdit *fDataFileSelector_lineEdit;
    QPushButton *fDataFileNameClear_pushButton;
    QWidget *layoutWidget_3;
    QHBoxLayout *horizontalLayout_24;
    QPushButton *fHelp_pushButton;
    QSpacerItem *horizontalSpacer_4;
    QPushButton *fResetAll_pushButton;
    QSpacerItem *horizontalSpacer_9;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *PGetMusrFTOptionsDialog)
    {
        if (PGetMusrFTOptionsDialog->objectName().isEmpty())
            PGetMusrFTOptionsDialog->setObjectName("PGetMusrFTOptionsDialog");
        PGetMusrFTOptionsDialog->setWindowModality(Qt::WindowModal);
        PGetMusrFTOptionsDialog->resize(711, 650);
        fFourier_groupBox = new QGroupBox(PGetMusrFTOptionsDialog);
        fFourier_groupBox->setObjectName("fFourier_groupBox");
        fFourier_groupBox->setGeometry(QRect(20, 400, 671, 201));
        QFont font;
        font.setBold(true);
        fFourier_groupBox->setFont(font);
        widget = new QWidget(fFourier_groupBox);
        widget->setObjectName("widget");
        widget->setGeometry(QRect(21, 31, 641, 156));
        verticalLayout_2 = new QVBoxLayout(widget);
        verticalLayout_2->setObjectName("verticalLayout_2");
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_22 = new QHBoxLayout();
        horizontalLayout_22->setObjectName("horizontalLayout_22");
        horizontalLayout_13 = new QHBoxLayout();
        horizontalLayout_13->setObjectName("horizontalLayout_13");
        fFourierOptions_label = new QLabel(widget);
        fFourierOptions_label->setObjectName("fFourierOptions_label");

        horizontalLayout_13->addWidget(fFourierOptions_label);

        fFourierOption_comboBox = new QComboBox(widget);
        fFourierOption_comboBox->addItem(QString());
        fFourierOption_comboBox->addItem(QString());
        fFourierOption_comboBox->addItem(QString());
        fFourierOption_comboBox->addItem(QString());
        fFourierOption_comboBox->addItem(QString());
        fFourierOption_comboBox->addItem(QString());
        fFourierOption_comboBox->setObjectName("fFourierOption_comboBox");

        horizontalLayout_13->addWidget(fFourierOption_comboBox);


        horizontalLayout_22->addLayout(horizontalLayout_13);

        horizontalSpacer_6 = new QSpacerItem(218, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_22->addItem(horizontalSpacer_6);

        horizontalLayout_14 = new QHBoxLayout();
        horizontalLayout_14->setObjectName("horizontalLayout_14");
        fFourierUnits_label = new QLabel(widget);
        fFourierUnits_label->setObjectName("fFourierUnits_label");

        horizontalLayout_14->addWidget(fFourierUnits_label);

        fFourierUnits_comboBox = new QComboBox(widget);
        fFourierUnits_comboBox->addItem(QString());
        fFourierUnits_comboBox->addItem(QString());
        fFourierUnits_comboBox->addItem(QString());
        fFourierUnits_comboBox->addItem(QString());
        fFourierUnits_comboBox->addItem(QString());
        fFourierUnits_comboBox->setObjectName("fFourierUnits_comboBox");

        horizontalLayout_14->addWidget(fFourierUnits_comboBox);


        horizontalLayout_22->addLayout(horizontalLayout_14);


        verticalLayout_2->addLayout(horizontalLayout_22);

        horizontalLayout_23 = new QHBoxLayout();
        horizontalLayout_23->setObjectName("horizontalLayout_23");
        horizontalLayout_19 = new QHBoxLayout();
        horizontalLayout_19->setObjectName("horizontalLayout_19");
        fFourierPower_label = new QLabel(widget);
        fFourierPower_label->setObjectName("fFourierPower_label");

        horizontalLayout_19->addWidget(fFourierPower_label);

        fFourierPower_lineEdit = new QLineEdit(widget);
        fFourierPower_lineEdit->setObjectName("fFourierPower_lineEdit");

        horizontalLayout_19->addWidget(fFourierPower_lineEdit);


        horizontalLayout_23->addLayout(horizontalLayout_19);

        horizontalSpacer_5 = new QSpacerItem(88, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_23->addItem(horizontalSpacer_5);

        horizontalLayout_17 = new QHBoxLayout();
        horizontalLayout_17->setObjectName("horizontalLayout_17");
        fLifetimeCorrection_label = new QLabel(widget);
        fLifetimeCorrection_label->setObjectName("fLifetimeCorrection_label");

        horizontalLayout_17->addWidget(fLifetimeCorrection_label);

        fLifetimeCorrection_lineEdit = new QLineEdit(widget);
        fLifetimeCorrection_lineEdit->setObjectName("fLifetimeCorrection_lineEdit");

        horizontalLayout_17->addWidget(fLifetimeCorrection_lineEdit);


        horizontalLayout_23->addLayout(horizontalLayout_17);


        verticalLayout_2->addLayout(horizontalLayout_23);

        horizontalLayout_20 = new QHBoxLayout();
        horizontalLayout_20->setObjectName("horizontalLayout_20");
        fFourierRange_label = new QLabel(widget);
        fFourierRange_label->setObjectName("fFourierRange_label");

        horizontalLayout_20->addWidget(fFourierRange_label);

        horizontalLayout_16 = new QHBoxLayout();
        horizontalLayout_16->setObjectName("horizontalLayout_16");
        fFourierRangeStart_label = new QLabel(widget);
        fFourierRangeStart_label->setObjectName("fFourierRangeStart_label");

        horizontalLayout_16->addWidget(fFourierRangeStart_label);

        fFourierRangeStart_lineEdit = new QLineEdit(widget);
        fFourierRangeStart_lineEdit->setObjectName("fFourierRangeStart_lineEdit");

        horizontalLayout_16->addWidget(fFourierRangeStart_lineEdit);


        horizontalLayout_20->addLayout(horizontalLayout_16);

        horizontalLayout_15 = new QHBoxLayout();
        horizontalLayout_15->setObjectName("horizontalLayout_15");
        fFourierRangeEnd_label = new QLabel(widget);
        fFourierRangeEnd_label->setObjectName("fFourierRangeEnd_label");

        horizontalLayout_15->addWidget(fFourierRangeEnd_label);

        fFourierRangeEnd_lineEdit = new QLineEdit(widget);
        fFourierRangeEnd_lineEdit->setObjectName("fFourierRangeEnd_lineEdit");

        horizontalLayout_15->addWidget(fFourierRangeEnd_lineEdit);


        horizontalLayout_20->addLayout(horizontalLayout_15);

        horizontalSpacer_3 = new QSpacerItem(118, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_20->addItem(horizontalSpacer_3);


        verticalLayout_2->addLayout(horizontalLayout_20);

        horizontalLayout_18 = new QHBoxLayout();
        horizontalLayout_18->setObjectName("horizontalLayout_18");
        fAveragedView_checkBox = new QCheckBox(widget);
        fAveragedView_checkBox->setObjectName("fAveragedView_checkBox");

        horizontalLayout_18->addWidget(fAveragedView_checkBox);

        fAveragePerDataSet_checkBox = new QCheckBox(widget);
        fAveragePerDataSet_checkBox->setObjectName("fAveragePerDataSet_checkBox");

        horizontalLayout_18->addWidget(fAveragePerDataSet_checkBox);

        horizontalSpacer_7 = new QSpacerItem(88, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_18->addItem(horizontalSpacer_7);

        fCreateMsrFile_checkBox = new QCheckBox(widget);
        fCreateMsrFile_checkBox->setObjectName("fCreateMsrFile_checkBox");

        horizontalLayout_18->addWidget(fCreateMsrFile_checkBox);


        verticalLayout_2->addLayout(horizontalLayout_18);

        horizontalLayout_21 = new QHBoxLayout();
        horizontalLayout_21->setObjectName("horizontalLayout_21");
        fFourierTitle_label = new QLabel(widget);
        fFourierTitle_label->setObjectName("fFourierTitle_label");

        horizontalLayout_21->addWidget(fFourierTitle_label);

        fFourierTitle_lineEdit = new QLineEdit(widget);
        fFourierTitle_lineEdit->setObjectName("fFourierTitle_lineEdit");

        horizontalLayout_21->addWidget(fFourierTitle_lineEdit);


        verticalLayout_2->addLayout(horizontalLayout_21);

        fHistoInfo_groupBox = new QGroupBox(PGetMusrFTOptionsDialog);
        fHistoInfo_groupBox->setObjectName("fHistoInfo_groupBox");
        fHistoInfo_groupBox->setGeometry(QRect(20, 160, 671, 231));
        fHistoInfo_groupBox->setFont(font);
        layoutWidget_0 = new QWidget(fHistoInfo_groupBox);
        layoutWidget_0->setObjectName("layoutWidget_0");
        layoutWidget_0->setGeometry(QRect(20, 30, 641, 188));
        verticalLayout = new QVBoxLayout(layoutWidget_0);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        fBkgRange_label = new QLabel(layoutWidget_0);
        fBkgRange_label->setObjectName("fBkgRange_label");
        fBkgRange_label->setFont(font);

        horizontalLayout_3->addWidget(fBkgRange_label);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        fBkgStartBin_label = new QLabel(layoutWidget_0);
        fBkgStartBin_label->setObjectName("fBkgStartBin_label");

        horizontalLayout->addWidget(fBkgStartBin_label);

        fBkgRangeStartBin_lineEdit = new QLineEdit(layoutWidget_0);
        fBkgRangeStartBin_lineEdit->setObjectName("fBkgRangeStartBin_lineEdit");

        horizontalLayout->addWidget(fBkgRangeStartBin_lineEdit);


        horizontalLayout_3->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        fBkgEndBin_label = new QLabel(layoutWidget_0);
        fBkgEndBin_label->setObjectName("fBkgEndBin_label");

        horizontalLayout_2->addWidget(fBkgEndBin_label);

        fBkgRangeEndBin_lineEdit = new QLineEdit(layoutWidget_0);
        fBkgRangeEndBin_lineEdit->setObjectName("fBkgRangeEndBin_lineEdit");

        horizontalLayout_2->addWidget(fBkgRangeEndBin_lineEdit);


        horizontalLayout_3->addLayout(horizontalLayout_2);


        verticalLayout->addLayout(horizontalLayout_3);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName("horizontalLayout_4");
        fBkgList_label = new QLabel(layoutWidget_0);
        fBkgList_label->setObjectName("fBkgList_label");

        horizontalLayout_4->addWidget(fBkgList_label);

        fBkgList_lineEdit = new QLineEdit(layoutWidget_0);
        fBkgList_lineEdit->setObjectName("fBkgList_lineEdit");

        horizontalLayout_4->addWidget(fBkgList_lineEdit);


        verticalLayout->addLayout(horizontalLayout_4);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setObjectName("horizontalLayout_7");
        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName("horizontalLayout_6");
        fApodization_label = new QLabel(layoutWidget_0);
        fApodization_label->setObjectName("fApodization_label");

        horizontalLayout_6->addWidget(fApodization_label);

        fApodization_comboBox = new QComboBox(layoutWidget_0);
        fApodization_comboBox->addItem(QString());
        fApodization_comboBox->addItem(QString());
        fApodization_comboBox->addItem(QString());
        fApodization_comboBox->addItem(QString());
        fApodization_comboBox->setObjectName("fApodization_comboBox");

        horizontalLayout_6->addWidget(fApodization_comboBox);


        horizontalLayout_7->addLayout(horizontalLayout_6);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName("horizontalLayout_5");
        fPacking_label = new QLabel(layoutWidget_0);
        fPacking_label->setObjectName("fPacking_label");

        horizontalLayout_5->addWidget(fPacking_label);

        fPacking_lineEdit = new QLineEdit(layoutWidget_0);
        fPacking_lineEdit->setObjectName("fPacking_lineEdit");

        horizontalLayout_5->addWidget(fPacking_lineEdit);


        horizontalLayout_7->addLayout(horizontalLayout_5);

        horizontalSpacer = new QSpacerItem(168, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_7->addItem(horizontalSpacer);


        verticalLayout->addLayout(horizontalLayout_7);

        horizontalLayout_10 = new QHBoxLayout();
        horizontalLayout_10->setObjectName("horizontalLayout_10");
        fTimeRange_label = new QLabel(layoutWidget_0);
        fTimeRange_label->setObjectName("fTimeRange_label");

        horizontalLayout_10->addWidget(fTimeRange_label);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_10->addItem(horizontalSpacer_2);

        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setObjectName("horizontalLayout_9");
        fTimeRangeStart_label = new QLabel(layoutWidget_0);
        fTimeRangeStart_label->setObjectName("fTimeRangeStart_label");

        horizontalLayout_9->addWidget(fTimeRangeStart_label);

        fTimeRangeStart_lineEdit = new QLineEdit(layoutWidget_0);
        fTimeRangeStart_lineEdit->setObjectName("fTimeRangeStart_lineEdit");

        horizontalLayout_9->addWidget(fTimeRangeStart_lineEdit);


        horizontalLayout_10->addLayout(horizontalLayout_9);

        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setObjectName("horizontalLayout_8");
        fTimeRangeEnd_label = new QLabel(layoutWidget_0);
        fTimeRangeEnd_label->setObjectName("fTimeRangeEnd_label");

        horizontalLayout_8->addWidget(fTimeRangeEnd_label);

        fTimeRangeEnd_lineEdit = new QLineEdit(layoutWidget_0);
        fTimeRangeEnd_lineEdit->setObjectName("fTimeRangeEnd_lineEdit");

        horizontalLayout_8->addWidget(fTimeRangeEnd_lineEdit);


        horizontalLayout_10->addLayout(horizontalLayout_8);


        verticalLayout->addLayout(horizontalLayout_10);

        horizontalLayout_11 = new QHBoxLayout();
        horizontalLayout_11->setObjectName("horizontalLayout_11");
        fHistoList_label = new QLabel(layoutWidget_0);
        fHistoList_label->setObjectName("fHistoList_label");

        horizontalLayout_11->addWidget(fHistoList_label);

        fHistoList_lineEdit = new QLineEdit(layoutWidget_0);
        fHistoList_lineEdit->setObjectName("fHistoList_lineEdit");

        horizontalLayout_11->addWidget(fHistoList_lineEdit);


        verticalLayout->addLayout(horizontalLayout_11);

        horizontalLayout_12 = new QHBoxLayout();
        horizontalLayout_12->setObjectName("horizontalLayout_12");
        fT0_label = new QLabel(layoutWidget_0);
        fT0_label->setObjectName("fT0_label");

        horizontalLayout_12->addWidget(fT0_label);

        fT0_lineEdit = new QLineEdit(layoutWidget_0);
        fT0_lineEdit->setObjectName("fT0_lineEdit");

        horizontalLayout_12->addWidget(fT0_lineEdit);


        verticalLayout->addLayout(horizontalLayout_12);

        fFileSelection_groupBox = new QGroupBox(PGetMusrFTOptionsDialog);
        fFileSelection_groupBox->setObjectName("fFileSelection_groupBox");
        fFileSelection_groupBox->setGeometry(QRect(20, 10, 671, 141));
        fFileSelection_groupBox->setFont(font);
        layoutWidget_1 = new QWidget(fFileSelection_groupBox);
        layoutWidget_1->setObjectName("layoutWidget_1");
        layoutWidget_1->setGeometry(QRect(19, 31, 641, 96));
        verticalLayout_3 = new QVBoxLayout(layoutWidget_1);
        verticalLayout_3->setObjectName("verticalLayout_3");
        verticalLayout_3->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_27 = new QHBoxLayout();
        horizontalLayout_27->setObjectName("horizontalLayout_27");
        fCurrentMsrFile_checkBox = new QCheckBox(layoutWidget_1);
        fCurrentMsrFile_checkBox->setObjectName("fCurrentMsrFile_checkBox");

        horizontalLayout_27->addWidget(fCurrentMsrFile_checkBox);

        fAllMsrFiles_checkBox = new QCheckBox(layoutWidget_1);
        fAllMsrFiles_checkBox->setObjectName("fAllMsrFiles_checkBox");

        horizontalLayout_27->addWidget(fAllMsrFiles_checkBox);

        horizontalSpacer_8 = new QSpacerItem(318, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_27->addItem(horizontalSpacer_8);


        verticalLayout_3->addLayout(horizontalLayout_27);

        horizontalLayout_25 = new QHBoxLayout();
        horizontalLayout_25->setObjectName("horizontalLayout_25");
        fMsrFileSelector_pushButton = new QPushButton(layoutWidget_1);
        fMsrFileSelector_pushButton->setObjectName("fMsrFileSelector_pushButton");

        horizontalLayout_25->addWidget(fMsrFileSelector_pushButton);

        fMsrFileSelector_lineEdit = new QLineEdit(layoutWidget_1);
        fMsrFileSelector_lineEdit->setObjectName("fMsrFileSelector_lineEdit");
        fMsrFileSelector_lineEdit->setMinimumSize(QSize(0, 23));

        horizontalLayout_25->addWidget(fMsrFileSelector_lineEdit);

        fMsrFileNameClear_pushButton = new QPushButton(layoutWidget_1);
        fMsrFileNameClear_pushButton->setObjectName("fMsrFileNameClear_pushButton");

        horizontalLayout_25->addWidget(fMsrFileNameClear_pushButton);


        verticalLayout_3->addLayout(horizontalLayout_25);

        horizontalLayout_26 = new QHBoxLayout();
        horizontalLayout_26->setObjectName("horizontalLayout_26");
        fDataFileSelector_pushButton = new QPushButton(layoutWidget_1);
        fDataFileSelector_pushButton->setObjectName("fDataFileSelector_pushButton");

        horizontalLayout_26->addWidget(fDataFileSelector_pushButton);

        fDataFileSelector_lineEdit = new QLineEdit(layoutWidget_1);
        fDataFileSelector_lineEdit->setObjectName("fDataFileSelector_lineEdit");
        fDataFileSelector_lineEdit->setMinimumSize(QSize(0, 23));

        horizontalLayout_26->addWidget(fDataFileSelector_lineEdit);

        fDataFileNameClear_pushButton = new QPushButton(layoutWidget_1);
        fDataFileNameClear_pushButton->setObjectName("fDataFileNameClear_pushButton");

        horizontalLayout_26->addWidget(fDataFileNameClear_pushButton);


        verticalLayout_3->addLayout(horizontalLayout_26);

        layoutWidget_3 = new QWidget(PGetMusrFTOptionsDialog);
        layoutWidget_3->setObjectName("layoutWidget_3");
        layoutWidget_3->setGeometry(QRect(20, 610, 671, 29));
        horizontalLayout_24 = new QHBoxLayout(layoutWidget_3);
        horizontalLayout_24->setObjectName("horizontalLayout_24");
        horizontalLayout_24->setContentsMargins(0, 0, 0, 0);
        fHelp_pushButton = new QPushButton(layoutWidget_3);
        fHelp_pushButton->setObjectName("fHelp_pushButton");

        horizontalLayout_24->addWidget(fHelp_pushButton);

        horizontalSpacer_4 = new QSpacerItem(78, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_24->addItem(horizontalSpacer_4);

        fResetAll_pushButton = new QPushButton(layoutWidget_3);
        fResetAll_pushButton->setObjectName("fResetAll_pushButton");
        fResetAll_pushButton->setFont(font);

        horizontalLayout_24->addWidget(fResetAll_pushButton);

        horizontalSpacer_9 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_24->addItem(horizontalSpacer_9);

        buttonBox = new QDialogButtonBox(layoutWidget_3);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        horizontalLayout_24->addWidget(buttonBox);

        QWidget::setTabOrder(fCurrentMsrFile_checkBox, fAllMsrFiles_checkBox);
        QWidget::setTabOrder(fAllMsrFiles_checkBox, fMsrFileSelector_pushButton);
        QWidget::setTabOrder(fMsrFileSelector_pushButton, fMsrFileSelector_lineEdit);
        QWidget::setTabOrder(fMsrFileSelector_lineEdit, fMsrFileNameClear_pushButton);
        QWidget::setTabOrder(fMsrFileNameClear_pushButton, fDataFileSelector_pushButton);
        QWidget::setTabOrder(fDataFileSelector_pushButton, fDataFileSelector_lineEdit);
        QWidget::setTabOrder(fDataFileSelector_lineEdit, fDataFileNameClear_pushButton);
        QWidget::setTabOrder(fDataFileNameClear_pushButton, fBkgRangeStartBin_lineEdit);
        QWidget::setTabOrder(fBkgRangeStartBin_lineEdit, fBkgRangeEndBin_lineEdit);
        QWidget::setTabOrder(fBkgRangeEndBin_lineEdit, fBkgList_lineEdit);
        QWidget::setTabOrder(fBkgList_lineEdit, fApodization_comboBox);
        QWidget::setTabOrder(fApodization_comboBox, fPacking_lineEdit);
        QWidget::setTabOrder(fPacking_lineEdit, fTimeRangeStart_lineEdit);
        QWidget::setTabOrder(fTimeRangeStart_lineEdit, fTimeRangeEnd_lineEdit);
        QWidget::setTabOrder(fTimeRangeEnd_lineEdit, fHistoList_lineEdit);
        QWidget::setTabOrder(fHistoList_lineEdit, fT0_lineEdit);
        QWidget::setTabOrder(fT0_lineEdit, fFourierOption_comboBox);
        QWidget::setTabOrder(fFourierOption_comboBox, fFourierUnits_comboBox);
        QWidget::setTabOrder(fFourierUnits_comboBox, fFourierPower_lineEdit);
        QWidget::setTabOrder(fFourierPower_lineEdit, fLifetimeCorrection_lineEdit);
        QWidget::setTabOrder(fLifetimeCorrection_lineEdit, fFourierRangeStart_lineEdit);
        QWidget::setTabOrder(fFourierRangeStart_lineEdit, fFourierRangeEnd_lineEdit);
        QWidget::setTabOrder(fFourierRangeEnd_lineEdit, fAveragedView_checkBox);
        QWidget::setTabOrder(fAveragedView_checkBox, fCreateMsrFile_checkBox);
        QWidget::setTabOrder(fCreateMsrFile_checkBox, fFourierTitle_lineEdit);
        QWidget::setTabOrder(fFourierTitle_lineEdit, fResetAll_pushButton);
        QWidget::setTabOrder(fResetAll_pushButton, fHelp_pushButton);
        QWidget::setTabOrder(fHelp_pushButton, buttonBox);

        retranslateUi(PGetMusrFTOptionsDialog);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, PGetMusrFTOptionsDialog, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, PGetMusrFTOptionsDialog, qOverload<>(&QDialog::reject));
        QObject::connect(fHelp_pushButton, SIGNAL(clicked()), PGetMusrFTOptionsDialog, SLOT(helpContent()));

        QMetaObject::connectSlotsByName(PGetMusrFTOptionsDialog);
    } // setupUi

    void retranslateUi(QDialog *PGetMusrFTOptionsDialog)
    {
        PGetMusrFTOptionsDialog->setWindowTitle(QCoreApplication::translate("PGetMusrFTOptionsDialog", "musrFT Options", nullptr));
        fFourier_groupBox->setTitle(QCoreApplication::translate("PGetMusrFTOptionsDialog", " Fourier ", nullptr));
        fFourierOptions_label->setText(QCoreApplication::translate("PGetMusrFTOptionsDialog", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:10pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-weight:600;\">Options</span></p></body></html>", nullptr));
        fFourierOption_comboBox->setItemText(0, QCoreApplication::translate("PGetMusrFTOptionsDialog", "UnDef", nullptr));
        fFourierOption_comboBox->setItemText(1, QCoreApplication::translate("PGetMusrFTOptionsDialog", "real", nullptr));
        fFourierOption_comboBox->setItemText(2, QCoreApplication::translate("PGetMusrFTOptionsDialog", "imag", nullptr));
        fFourierOption_comboBox->setItemText(3, QCoreApplication::translate("PGetMusrFTOptionsDialog", "real+imag", nullptr));
        fFourierOption_comboBox->setItemText(4, QCoreApplication::translate("PGetMusrFTOptionsDialog", "power", nullptr));
        fFourierOption_comboBox->setItemText(5, QCoreApplication::translate("PGetMusrFTOptionsDialog", "phase", nullptr));

        fFourierUnits_label->setText(QCoreApplication::translate("PGetMusrFTOptionsDialog", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:10pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-weight:600;\">Units</span></p></body></html>", nullptr));
        fFourierUnits_comboBox->setItemText(0, QCoreApplication::translate("PGetMusrFTOptionsDialog", "UnDef", nullptr));
        fFourierUnits_comboBox->setItemText(1, QCoreApplication::translate("PGetMusrFTOptionsDialog", "Gauss", nullptr));
        fFourierUnits_comboBox->setItemText(2, QCoreApplication::translate("PGetMusrFTOptionsDialog", "Tesla", nullptr));
        fFourierUnits_comboBox->setItemText(3, QCoreApplication::translate("PGetMusrFTOptionsDialog", "MHz", nullptr));
        fFourierUnits_comboBox->setItemText(4, QCoreApplication::translate("PGetMusrFTOptionsDialog", "Mc/s", nullptr));

        fFourierPower_label->setText(QCoreApplication::translate("PGetMusrFTOptionsDialog", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:10pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-weight:600;\">Power</span></p></body></html>", nullptr));
        fLifetimeCorrection_label->setText(QCoreApplication::translate("PGetMusrFTOptionsDialog", "Lifetime Correction", nullptr));
        fFourierRange_label->setText(QCoreApplication::translate("PGetMusrFTOptionsDialog", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:10pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-weight:600;\">Range</span></p></body></html>", nullptr));
        fFourierRangeStart_label->setText(QCoreApplication::translate("PGetMusrFTOptionsDialog", "Start", nullptr));
        fFourierRangeEnd_label->setText(QCoreApplication::translate("PGetMusrFTOptionsDialog", "End", nullptr));
        fAveragedView_checkBox->setText(QCoreApplication::translate("PGetMusrFTOptionsDialog", "Average All", nullptr));
        fAveragePerDataSet_checkBox->setText(QCoreApplication::translate("PGetMusrFTOptionsDialog", "Average per Data Set", nullptr));
        fCreateMsrFile_checkBox->setText(QCoreApplication::translate("PGetMusrFTOptionsDialog", "Create msr-File", nullptr));
        fFourierTitle_label->setText(QCoreApplication::translate("PGetMusrFTOptionsDialog", "Title", nullptr));
        fHistoInfo_groupBox->setTitle(QCoreApplication::translate("PGetMusrFTOptionsDialog", " Histo Info ", nullptr));
        fBkgRange_label->setText(QCoreApplication::translate("PGetMusrFTOptionsDialog", "Background Range (in Bins)", nullptr));
        fBkgStartBin_label->setText(QCoreApplication::translate("PGetMusrFTOptionsDialog", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:10pt; font-weight:600; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">StartBin</p></body></html>", nullptr));
        fBkgEndBin_label->setText(QCoreApplication::translate("PGetMusrFTOptionsDialog", "EndBin:", nullptr));
        fBkgList_label->setText(QCoreApplication::translate("PGetMusrFTOptionsDialog", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:10pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-weight:600;\">Background List</span></p></body></html>", nullptr));
        fApodization_label->setText(QCoreApplication::translate("PGetMusrFTOptionsDialog", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:10pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-weight:600;\">Apodization</span></p></body></html>", nullptr));
        fApodization_comboBox->setItemText(0, QCoreApplication::translate("PGetMusrFTOptionsDialog", "none", nullptr));
        fApodization_comboBox->setItemText(1, QCoreApplication::translate("PGetMusrFTOptionsDialog", "weak", nullptr));
        fApodization_comboBox->setItemText(2, QCoreApplication::translate("PGetMusrFTOptionsDialog", "medium", nullptr));
        fApodization_comboBox->setItemText(3, QCoreApplication::translate("PGetMusrFTOptionsDialog", "strong", nullptr));

        fPacking_label->setText(QCoreApplication::translate("PGetMusrFTOptionsDialog", "Packing", nullptr));
        fTimeRange_label->setText(QCoreApplication::translate("PGetMusrFTOptionsDialog", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:10pt; font-weight:600; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Time Range (usec)</p></body></html>", nullptr));
        fTimeRangeStart_label->setText(QCoreApplication::translate("PGetMusrFTOptionsDialog", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:10pt; font-weight:600; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Start</p></body></html>", nullptr));
        fTimeRangeEnd_label->setText(QCoreApplication::translate("PGetMusrFTOptionsDialog", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:10pt; font-weight:600; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">End</p></body></html>", nullptr));
        fHistoList_label->setText(QCoreApplication::translate("PGetMusrFTOptionsDialog", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:10pt; font-weight:600; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Histo List</p></body></html>", nullptr));
        fT0_label->setText(QCoreApplication::translate("PGetMusrFTOptionsDialog", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:10pt; font-weight:600; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">T0's</p></body></html>", nullptr));
        fFileSelection_groupBox->setTitle(QCoreApplication::translate("PGetMusrFTOptionsDialog", "File Selection", nullptr));
        fCurrentMsrFile_checkBox->setText(QCoreApplication::translate("PGetMusrFTOptionsDialog", "Current msr-File", nullptr));
        fAllMsrFiles_checkBox->setText(QCoreApplication::translate("PGetMusrFTOptionsDialog", "ALL msr-Files", nullptr));
        fMsrFileSelector_pushButton->setText(QCoreApplication::translate("PGetMusrFTOptionsDialog", "Select msr-File(s)", nullptr));
#if QT_CONFIG(tooltip)
        fMsrFileNameClear_pushButton->setToolTip(QCoreApplication::translate("PGetMusrFTOptionsDialog", "clear msr-file name list", nullptr));
#endif // QT_CONFIG(tooltip)
        fMsrFileNameClear_pushButton->setText(QCoreApplication::translate("PGetMusrFTOptionsDialog", "Clear", nullptr));
        fDataFileSelector_pushButton->setText(QCoreApplication::translate("PGetMusrFTOptionsDialog", "Select data-File(s)", nullptr));
#if QT_CONFIG(tooltip)
        fDataFileNameClear_pushButton->setToolTip(QCoreApplication::translate("PGetMusrFTOptionsDialog", "clear data-file name list", nullptr));
#endif // QT_CONFIG(tooltip)
        fDataFileNameClear_pushButton->setText(QCoreApplication::translate("PGetMusrFTOptionsDialog", "Clear", nullptr));
        fHelp_pushButton->setText(QCoreApplication::translate("PGetMusrFTOptionsDialog", "Help", nullptr));
        fResetAll_pushButton->setText(QCoreApplication::translate("PGetMusrFTOptionsDialog", "Reset All", nullptr));
    } // retranslateUi

};

namespace Ui {
    class PGetMusrFTOptionsDialog: public Ui_PGetMusrFTOptionsDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PGETMUSRFTOPTIONSDIALOG_H
