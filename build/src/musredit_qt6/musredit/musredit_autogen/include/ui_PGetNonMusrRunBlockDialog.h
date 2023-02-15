/********************************************************************************
** Form generated from reading UI file 'PGetNonMusrRunBlockDialog.ui'
**
** Created by: Qt User Interface Compiler version 6.4.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PGETNONMUSRRUNBLOCKDIALOG_H
#define UI_PGETNONMUSRRUNBLOCKDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PGetNonMusrRunBlockDialog
{
public:
    QGroupBox *fRunHeaderInfo_groupBox;
    QWidget *layoutWidget_0;
    QHBoxLayout *horizontalLayout_2;
    QLabel *fRunFileName_label;
    QLineEdit *fRunFileName_lineEdit;
    QWidget *layoutWidget_1;
    QHBoxLayout *horizontalLayout_3;
    QLabel *fBeamline_label;
    QSpacerItem *horizontalSpacer_3;
    QLineEdit *fBeamline_lineEdit;
    QSpacerItem *horizontalSpacer_2;
    QWidget *layoutWidget_2;
    QHBoxLayout *horizontalLayout_4;
    QLabel *fInstitute_label;
    QSpacerItem *horizontalSpacer_4;
    QComboBox *fInstitute_comboBox;
    QSpacerItem *horizontalSpacer_5;
    QLabel *fFileFormat_label;
    QComboBox *fFileFormat_comboBox;
    QGroupBox *fReqiredEntries_groupBox;
    QWidget *layoutWidget_3;
    QHBoxLayout *horizontalLayout_5;
    QLabel *fMap_label;
    QLineEdit *fMap_lineEdit;
    QWidget *layoutWidget_4;
    QHBoxLayout *horizontalLayout_6;
    QLabel *fXYData_label;
    QSpacerItem *horizontalSpacer_6;
    QLineEdit *fXData_lineEdit;
    QLineEdit *fYData_lineEdit;
    QWidget *layoutWidget_5;
    QHBoxLayout *horizontalLayout_7;
    QLabel *fFitRange_label;
    QLineEdit *fFitRangeStart_lineEdit;
    QLineEdit *fFitRangeEnd_lineEdit;
    QWidget *layoutWidget_6;
    QHBoxLayout *horizontalLayout;
    QPushButton *fHelp_pushButton;
    QSpacerItem *horizontalSpacer;
    QPushButton *fOk_pushButton;
    QPushButton *fCancel_pushButton;

    void setupUi(QDialog *PGetNonMusrRunBlockDialog)
    {
        if (PGetNonMusrRunBlockDialog->objectName().isEmpty())
            PGetNonMusrRunBlockDialog->setObjectName("PGetNonMusrRunBlockDialog");
        PGetNonMusrRunBlockDialog->setWindowModality(Qt::WindowModal);
        PGetNonMusrRunBlockDialog->resize(419, 307);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/images/musrfit.xpm"), QSize(), QIcon::Normal, QIcon::Off);
        PGetNonMusrRunBlockDialog->setWindowIcon(icon);
        fRunHeaderInfo_groupBox = new QGroupBox(PGetNonMusrRunBlockDialog);
        fRunHeaderInfo_groupBox->setObjectName("fRunHeaderInfo_groupBox");
        fRunHeaderInfo_groupBox->setGeometry(QRect(10, 0, 401, 131));
        layoutWidget_0 = new QWidget(fRunHeaderInfo_groupBox);
        layoutWidget_0->setObjectName("layoutWidget_0");
        layoutWidget_0->setGeometry(QRect(10, 30, 381, 28));
        horizontalLayout_2 = new QHBoxLayout(layoutWidget_0);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        fRunFileName_label = new QLabel(layoutWidget_0);
        fRunFileName_label->setObjectName("fRunFileName_label");

        horizontalLayout_2->addWidget(fRunFileName_label);

        fRunFileName_lineEdit = new QLineEdit(layoutWidget_0);
        fRunFileName_lineEdit->setObjectName("fRunFileName_lineEdit");

        horizontalLayout_2->addWidget(fRunFileName_lineEdit);

        layoutWidget_1 = new QWidget(fRunHeaderInfo_groupBox);
        layoutWidget_1->setObjectName("layoutWidget_1");
        layoutWidget_1->setGeometry(QRect(11, 60, 381, 28));
        horizontalLayout_3 = new QHBoxLayout(layoutWidget_1);
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        horizontalLayout_3->setContentsMargins(0, 0, 0, 0);
        fBeamline_label = new QLabel(layoutWidget_1);
        fBeamline_label->setObjectName("fBeamline_label");

        horizontalLayout_3->addWidget(fBeamline_label);

        horizontalSpacer_3 = new QSpacerItem(23, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_3);

        fBeamline_lineEdit = new QLineEdit(layoutWidget_1);
        fBeamline_lineEdit->setObjectName("fBeamline_lineEdit");

        horizontalLayout_3->addWidget(fBeamline_lineEdit);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_2);

        layoutWidget_2 = new QWidget(fRunHeaderInfo_groupBox);
        layoutWidget_2->setObjectName("layoutWidget_2");
        layoutWidget_2->setGeometry(QRect(10, 91, 381, 27));
        horizontalLayout_4 = new QHBoxLayout(layoutWidget_2);
        horizontalLayout_4->setObjectName("horizontalLayout_4");
        horizontalLayout_4->setContentsMargins(0, 0, 0, 0);
        fInstitute_label = new QLabel(layoutWidget_2);
        fInstitute_label->setObjectName("fInstitute_label");

        horizontalLayout_4->addWidget(fInstitute_label);

        horizontalSpacer_4 = new QSpacerItem(34, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_4);

        fInstitute_comboBox = new QComboBox(layoutWidget_2);
        fInstitute_comboBox->addItem(QString());
        fInstitute_comboBox->addItem(QString());
        fInstitute_comboBox->addItem(QString());
        fInstitute_comboBox->addItem(QString());
        fInstitute_comboBox->setObjectName("fInstitute_comboBox");

        horizontalLayout_4->addWidget(fInstitute_comboBox);

        horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_5);

        fFileFormat_label = new QLabel(layoutWidget_2);
        fFileFormat_label->setObjectName("fFileFormat_label");

        horizontalLayout_4->addWidget(fFileFormat_label);

        fFileFormat_comboBox = new QComboBox(layoutWidget_2);
        fFileFormat_comboBox->addItem(QString());
        fFileFormat_comboBox->addItem(QString());
        fFileFormat_comboBox->setObjectName("fFileFormat_comboBox");

        horizontalLayout_4->addWidget(fFileFormat_comboBox);

        fReqiredEntries_groupBox = new QGroupBox(PGetNonMusrRunBlockDialog);
        fReqiredEntries_groupBox->setObjectName("fReqiredEntries_groupBox");
        fReqiredEntries_groupBox->setGeometry(QRect(10, 130, 401, 131));
        layoutWidget_3 = new QWidget(fReqiredEntries_groupBox);
        layoutWidget_3->setObjectName("layoutWidget_3");
        layoutWidget_3->setGeometry(QRect(10, 30, 381, 28));
        horizontalLayout_5 = new QHBoxLayout(layoutWidget_3);
        horizontalLayout_5->setObjectName("horizontalLayout_5");
        horizontalLayout_5->setContentsMargins(0, 0, 0, 0);
        fMap_label = new QLabel(layoutWidget_3);
        fMap_label->setObjectName("fMap_label");

        horizontalLayout_5->addWidget(fMap_label);

        fMap_lineEdit = new QLineEdit(layoutWidget_3);
        fMap_lineEdit->setObjectName("fMap_lineEdit");

        horizontalLayout_5->addWidget(fMap_lineEdit);

        layoutWidget_4 = new QWidget(fReqiredEntries_groupBox);
        layoutWidget_4->setObjectName("layoutWidget_4");
        layoutWidget_4->setGeometry(QRect(11, 60, 281, 28));
        horizontalLayout_6 = new QHBoxLayout(layoutWidget_4);
        horizontalLayout_6->setObjectName("horizontalLayout_6");
        horizontalLayout_6->setContentsMargins(0, 0, 0, 0);
        fXYData_label = new QLabel(layoutWidget_4);
        fXYData_label->setObjectName("fXYData_label");

        horizontalLayout_6->addWidget(fXYData_label);

        horizontalSpacer_6 = new QSpacerItem(37, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_6->addItem(horizontalSpacer_6);

        fXData_lineEdit = new QLineEdit(layoutWidget_4);
        fXData_lineEdit->setObjectName("fXData_lineEdit");

        horizontalLayout_6->addWidget(fXData_lineEdit);

        fYData_lineEdit = new QLineEdit(layoutWidget_4);
        fYData_lineEdit->setObjectName("fYData_lineEdit");

        horizontalLayout_6->addWidget(fYData_lineEdit);

        layoutWidget_5 = new QWidget(fReqiredEntries_groupBox);
        layoutWidget_5->setObjectName("layoutWidget_5");
        layoutWidget_5->setGeometry(QRect(10, 92, 291, 28));
        horizontalLayout_7 = new QHBoxLayout(layoutWidget_5);
        horizontalLayout_7->setObjectName("horizontalLayout_7");
        horizontalLayout_7->setContentsMargins(0, 0, 0, 0);
        fFitRange_label = new QLabel(layoutWidget_5);
        fFitRange_label->setObjectName("fFitRange_label");
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(fFitRange_label->sizePolicy().hasHeightForWidth());
        fFitRange_label->setSizePolicy(sizePolicy);

        horizontalLayout_7->addWidget(fFitRange_label);

        fFitRangeStart_lineEdit = new QLineEdit(layoutWidget_5);
        fFitRangeStart_lineEdit->setObjectName("fFitRangeStart_lineEdit");
        sizePolicy.setHeightForWidth(fFitRangeStart_lineEdit->sizePolicy().hasHeightForWidth());
        fFitRangeStart_lineEdit->setSizePolicy(sizePolicy);
        fFitRangeStart_lineEdit->setMinimumSize(QSize(105, 0));
        fFitRangeStart_lineEdit->setMaximumSize(QSize(105, 16777215));

        horizontalLayout_7->addWidget(fFitRangeStart_lineEdit);

        fFitRangeEnd_lineEdit = new QLineEdit(layoutWidget_5);
        fFitRangeEnd_lineEdit->setObjectName("fFitRangeEnd_lineEdit");
        sizePolicy.setHeightForWidth(fFitRangeEnd_lineEdit->sizePolicy().hasHeightForWidth());
        fFitRangeEnd_lineEdit->setSizePolicy(sizePolicy);
        fFitRangeEnd_lineEdit->setMinimumSize(QSize(105, 0));
        fFitRangeEnd_lineEdit->setMaximumSize(QSize(105, 16777215));

        horizontalLayout_7->addWidget(fFitRangeEnd_lineEdit);

        layoutWidget_6 = new QWidget(PGetNonMusrRunBlockDialog);
        layoutWidget_6->setObjectName("layoutWidget_6");
        layoutWidget_6->setGeometry(QRect(10, 270, 401, 27));
        horizontalLayout = new QHBoxLayout(layoutWidget_6);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        fHelp_pushButton = new QPushButton(layoutWidget_6);
        fHelp_pushButton->setObjectName("fHelp_pushButton");

        horizontalLayout->addWidget(fHelp_pushButton);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        fOk_pushButton = new QPushButton(layoutWidget_6);
        fOk_pushButton->setObjectName("fOk_pushButton");

        horizontalLayout->addWidget(fOk_pushButton);

        fCancel_pushButton = new QPushButton(layoutWidget_6);
        fCancel_pushButton->setObjectName("fCancel_pushButton");

        horizontalLayout->addWidget(fCancel_pushButton);

        QWidget::setTabOrder(fRunFileName_lineEdit, fBeamline_lineEdit);
        QWidget::setTabOrder(fBeamline_lineEdit, fInstitute_comboBox);
        QWidget::setTabOrder(fInstitute_comboBox, fFileFormat_comboBox);
        QWidget::setTabOrder(fFileFormat_comboBox, fMap_lineEdit);
        QWidget::setTabOrder(fMap_lineEdit, fXData_lineEdit);
        QWidget::setTabOrder(fXData_lineEdit, fYData_lineEdit);
        QWidget::setTabOrder(fYData_lineEdit, fFitRangeStart_lineEdit);
        QWidget::setTabOrder(fFitRangeStart_lineEdit, fFitRangeEnd_lineEdit);
        QWidget::setTabOrder(fFitRangeEnd_lineEdit, fOk_pushButton);
        QWidget::setTabOrder(fOk_pushButton, fCancel_pushButton);
        QWidget::setTabOrder(fCancel_pushButton, fHelp_pushButton);

        retranslateUi(PGetNonMusrRunBlockDialog);
        QObject::connect(fCancel_pushButton, &QPushButton::clicked, PGetNonMusrRunBlockDialog, qOverload<>(&QDialog::reject));
        QObject::connect(fOk_pushButton, &QPushButton::clicked, PGetNonMusrRunBlockDialog, qOverload<>(&QDialog::accept));
        QObject::connect(fHelp_pushButton, SIGNAL(clicked()), PGetNonMusrRunBlockDialog, SLOT(helpContent()));

        QMetaObject::connectSlotsByName(PGetNonMusrRunBlockDialog);
    } // setupUi

    void retranslateUi(QDialog *PGetNonMusrRunBlockDialog)
    {
        PGetNonMusrRunBlockDialog->setWindowTitle(QCoreApplication::translate("PGetNonMusrRunBlockDialog", "Get NonMusr Run Block Parameters", nullptr));
        fRunHeaderInfo_groupBox->setTitle(QCoreApplication::translate("PGetNonMusrRunBlockDialog", "Run Header Info", nullptr));
        fRunFileName_label->setText(QCoreApplication::translate("PGetNonMusrRunBlockDialog", "Run File Name", nullptr));
        fBeamline_label->setText(QCoreApplication::translate("PGetNonMusrRunBlockDialog", "Beamline", nullptr));
        fInstitute_label->setText(QCoreApplication::translate("PGetNonMusrRunBlockDialog", "Institute", nullptr));
        fInstitute_comboBox->setItemText(0, QCoreApplication::translate("PGetNonMusrRunBlockDialog", "PSI", nullptr));
        fInstitute_comboBox->setItemText(1, QCoreApplication::translate("PGetNonMusrRunBlockDialog", "RAL", nullptr));
        fInstitute_comboBox->setItemText(2, QCoreApplication::translate("PGetNonMusrRunBlockDialog", "TRIUMF", nullptr));
        fInstitute_comboBox->setItemText(3, QCoreApplication::translate("PGetNonMusrRunBlockDialog", "JPARC", nullptr));

        fFileFormat_label->setText(QCoreApplication::translate("PGetNonMusrRunBlockDialog", "File Format", nullptr));
        fFileFormat_comboBox->setItemText(0, QCoreApplication::translate("PGetNonMusrRunBlockDialog", "ASCII", nullptr));
        fFileFormat_comboBox->setItemText(1, QCoreApplication::translate("PGetNonMusrRunBlockDialog", "DB", nullptr));

        fReqiredEntries_groupBox->setTitle(QCoreApplication::translate("PGetNonMusrRunBlockDialog", "Required Entries", nullptr));
        fMap_label->setText(QCoreApplication::translate("PGetNonMusrRunBlockDialog", "Map", nullptr));
        fMap_lineEdit->setText(QCoreApplication::translate("PGetNonMusrRunBlockDialog", "0  0  0  0  0  0  0  0  0  0", nullptr));
        fXYData_label->setText(QCoreApplication::translate("PGetNonMusrRunBlockDialog", "xy-Data", nullptr));
        fFitRange_label->setText(QCoreApplication::translate("PGetNonMusrRunBlockDialog", "Fit Range", nullptr));
        fHelp_pushButton->setText(QCoreApplication::translate("PGetNonMusrRunBlockDialog", "&Help", nullptr));
        fOk_pushButton->setText(QCoreApplication::translate("PGetNonMusrRunBlockDialog", "&OK", nullptr));
        fCancel_pushButton->setText(QCoreApplication::translate("PGetNonMusrRunBlockDialog", "&Cancel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class PGetNonMusrRunBlockDialog: public Ui_PGetNonMusrRunBlockDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PGETNONMUSRRUNBLOCKDIALOG_H
