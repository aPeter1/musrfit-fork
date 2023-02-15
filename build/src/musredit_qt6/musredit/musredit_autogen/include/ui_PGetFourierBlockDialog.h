/********************************************************************************
** Form generated from reading UI file 'PGetFourierBlockDialog.ui'
**
** Created by: Qt User Interface Compiler version 6.4.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PGETFOURIERBLOCKDIALOG_H
#define UI_PGETFOURIERBLOCKDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PGetFourierBlockDialog
{
public:
    QWidget *layoutWidget;
    QHBoxLayout *horizontalLayout;
    QPushButton *fHelp_pushButton;
    QSpacerItem *horizontalSpacer;
    QPushButton *fOk_pushButton;
    QPushButton *fCancel_pushButton;
    QLabel *fUnits_label;
    QLabel *fFourierPower_label;
    QLabel *fApodization_label;
    QLabel *fPlot_label;
    QLabel *fPhase_label;
    QLabel *fPhaseCorrectionRange_label;
    QLabel *fRange_label;
    QComboBox *fUnits_comboBox;
    QLineEdit *fFourierPower_lineEdit;
    QComboBox *fApodization_comboBox;
    QComboBox *fPlot_comboBox;
    QLineEdit *fPhase_lineEdit;
    QLineEdit *fPhaseCorrectionRangeLow_lineEdit;
    QLineEdit *fPhaseCorrectionRangeUp_lineEdit;
    QLineEdit *fRangeLow_lineEdit;
    QLineEdit *fRangeUp_lineEdit;

    void setupUi(QDialog *PGetFourierBlockDialog)
    {
        if (PGetFourierBlockDialog->objectName().isEmpty())
            PGetFourierBlockDialog->setObjectName("PGetFourierBlockDialog");
        PGetFourierBlockDialog->setWindowModality(Qt::WindowModal);
        PGetFourierBlockDialog->resize(367, 269);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/images/musrfit.xpm"), QSize(), QIcon::Normal, QIcon::Off);
        PGetFourierBlockDialog->setWindowIcon(icon);
        layoutWidget = new QWidget(PGetFourierBlockDialog);
        layoutWidget->setObjectName("layoutWidget");
        layoutWidget->setGeometry(QRect(10, 230, 351, 32));
        horizontalLayout = new QHBoxLayout(layoutWidget);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        fHelp_pushButton = new QPushButton(layoutWidget);
        fHelp_pushButton->setObjectName("fHelp_pushButton");

        horizontalLayout->addWidget(fHelp_pushButton);

        horizontalSpacer = new QSpacerItem(118, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        fOk_pushButton = new QPushButton(layoutWidget);
        fOk_pushButton->setObjectName("fOk_pushButton");

        horizontalLayout->addWidget(fOk_pushButton);

        fCancel_pushButton = new QPushButton(layoutWidget);
        fCancel_pushButton->setObjectName("fCancel_pushButton");

        horizontalLayout->addWidget(fCancel_pushButton);

        fUnits_label = new QLabel(PGetFourierBlockDialog);
        fUnits_label->setObjectName("fUnits_label");
        fUnits_label->setGeometry(QRect(10, 10, 61, 17));
        fFourierPower_label = new QLabel(PGetFourierBlockDialog);
        fFourierPower_label->setObjectName("fFourierPower_label");
        fFourierPower_label->setGeometry(QRect(10, 45, 91, 17));
        fApodization_label = new QLabel(PGetFourierBlockDialog);
        fApodization_label->setObjectName("fApodization_label");
        fApodization_label->setGeometry(QRect(10, 75, 141, 17));
        fPlot_label = new QLabel(PGetFourierBlockDialog);
        fPlot_label->setObjectName("fPlot_label");
        fPlot_label->setGeometry(QRect(10, 110, 61, 17));
        fPhase_label = new QLabel(PGetFourierBlockDialog);
        fPhase_label->setObjectName("fPhase_label");
        fPhase_label->setGeometry(QRect(10, 140, 61, 17));
        fPhaseCorrectionRange_label = new QLabel(PGetFourierBlockDialog);
        fPhaseCorrectionRange_label->setObjectName("fPhaseCorrectionRange_label");
        fPhaseCorrectionRange_label->setGeometry(QRect(0, 160, 111, 41));
        fRange_label = new QLabel(PGetFourierBlockDialog);
        fRange_label->setObjectName("fRange_label");
        fRange_label->setGeometry(QRect(10, 200, 61, 17));
        fUnits_comboBox = new QComboBox(PGetFourierBlockDialog);
        fUnits_comboBox->addItem(QString());
        fUnits_comboBox->addItem(QString());
        fUnits_comboBox->addItem(QString());
        fUnits_comboBox->addItem(QString());
        fUnits_comboBox->setObjectName("fUnits_comboBox");
        fUnits_comboBox->setGeometry(QRect(130, 5, 111, 26));
        fFourierPower_lineEdit = new QLineEdit(PGetFourierBlockDialog);
        fFourierPower_lineEdit->setObjectName("fFourierPower_lineEdit");
        fFourierPower_lineEdit->setGeometry(QRect(130, 40, 113, 22));
        fApodization_comboBox = new QComboBox(PGetFourierBlockDialog);
        fApodization_comboBox->addItem(QString());
        fApodization_comboBox->addItem(QString());
        fApodization_comboBox->addItem(QString());
        fApodization_comboBox->addItem(QString());
        fApodization_comboBox->setObjectName("fApodization_comboBox");
        fApodization_comboBox->setGeometry(QRect(130, 70, 111, 26));
        fPlot_comboBox = new QComboBox(PGetFourierBlockDialog);
        fPlot_comboBox->addItem(QString());
        fPlot_comboBox->addItem(QString());
        fPlot_comboBox->addItem(QString());
        fPlot_comboBox->addItem(QString());
        fPlot_comboBox->addItem(QString());
        fPlot_comboBox->setObjectName("fPlot_comboBox");
        fPlot_comboBox->setGeometry(QRect(130, 105, 111, 26));
        fPhase_lineEdit = new QLineEdit(PGetFourierBlockDialog);
        fPhase_lineEdit->setObjectName("fPhase_lineEdit");
        fPhase_lineEdit->setGeometry(QRect(130, 140, 113, 22));
        fPhaseCorrectionRangeLow_lineEdit = new QLineEdit(PGetFourierBlockDialog);
        fPhaseCorrectionRangeLow_lineEdit->setObjectName("fPhaseCorrectionRangeLow_lineEdit");
        fPhaseCorrectionRangeLow_lineEdit->setGeometry(QRect(130, 170, 113, 22));
        fPhaseCorrectionRangeUp_lineEdit = new QLineEdit(PGetFourierBlockDialog);
        fPhaseCorrectionRangeUp_lineEdit->setObjectName("fPhaseCorrectionRangeUp_lineEdit");
        fPhaseCorrectionRangeUp_lineEdit->setGeometry(QRect(250, 170, 113, 22));
        fRangeLow_lineEdit = new QLineEdit(PGetFourierBlockDialog);
        fRangeLow_lineEdit->setObjectName("fRangeLow_lineEdit");
        fRangeLow_lineEdit->setGeometry(QRect(130, 200, 113, 22));
        fRangeUp_lineEdit = new QLineEdit(PGetFourierBlockDialog);
        fRangeUp_lineEdit->setObjectName("fRangeUp_lineEdit");
        fRangeUp_lineEdit->setGeometry(QRect(250, 200, 113, 22));
        QWidget::setTabOrder(fOk_pushButton, fCancel_pushButton);
        QWidget::setTabOrder(fCancel_pushButton, fHelp_pushButton);

        retranslateUi(PGetFourierBlockDialog);
        QObject::connect(fCancel_pushButton, &QPushButton::clicked, PGetFourierBlockDialog, qOverload<>(&QDialog::reject));
        QObject::connect(fOk_pushButton, &QPushButton::clicked, PGetFourierBlockDialog, qOverload<>(&QDialog::accept));
        QObject::connect(fHelp_pushButton, SIGNAL(clicked()), PGetFourierBlockDialog, SLOT(helpContent()));
        QObject::connect(fOk_pushButton, SIGNAL(clicked()), PGetFourierBlockDialog, SLOT(fillFourierBlock()));

        QMetaObject::connectSlotsByName(PGetFourierBlockDialog);
    } // setupUi

    void retranslateUi(QDialog *PGetFourierBlockDialog)
    {
        PGetFourierBlockDialog->setWindowTitle(QCoreApplication::translate("PGetFourierBlockDialog", "Get Fourier", nullptr));
        fHelp_pushButton->setText(QCoreApplication::translate("PGetFourierBlockDialog", "&Help", nullptr));
        fOk_pushButton->setText(QCoreApplication::translate("PGetFourierBlockDialog", "&OK", nullptr));
        fCancel_pushButton->setText(QCoreApplication::translate("PGetFourierBlockDialog", "&Cancel", nullptr));
        fUnits_label->setText(QCoreApplication::translate("PGetFourierBlockDialog", "Units", nullptr));
        fFourierPower_label->setText(QCoreApplication::translate("PGetFourierBlockDialog", "Fourier Power", nullptr));
        fApodization_label->setText(QCoreApplication::translate("PGetFourierBlockDialog", "Apodization", nullptr));
        fPlot_label->setText(QCoreApplication::translate("PGetFourierBlockDialog", "Plot", nullptr));
        fPhase_label->setText(QCoreApplication::translate("PGetFourierBlockDialog", "Phase", nullptr));
        fPhaseCorrectionRange_label->setText(QCoreApplication::translate("PGetFourierBlockDialog", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'Lucida Grande'; font-size:8pt;\">Phase Correction</span></p>\n"
"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'Lucida Grande'; font-size:8pt;\">Range</span></p></body></html>", nullptr));
        fRange_label->setText(QCoreApplication::translate("PGetFourierBlockDialog", "Range", nullptr));
        fUnits_comboBox->setItemText(0, QCoreApplication::translate("PGetFourierBlockDialog", "Gauss", nullptr));
        fUnits_comboBox->setItemText(1, QCoreApplication::translate("PGetFourierBlockDialog", "Tesla", nullptr));
        fUnits_comboBox->setItemText(2, QCoreApplication::translate("PGetFourierBlockDialog", "MHz", nullptr));
        fUnits_comboBox->setItemText(3, QCoreApplication::translate("PGetFourierBlockDialog", "Mc/s", nullptr));

        fApodization_comboBox->setItemText(0, QCoreApplication::translate("PGetFourierBlockDialog", "None", nullptr));
        fApodization_comboBox->setItemText(1, QCoreApplication::translate("PGetFourierBlockDialog", "Weak", nullptr));
        fApodization_comboBox->setItemText(2, QCoreApplication::translate("PGetFourierBlockDialog", "Medium", nullptr));
        fApodization_comboBox->setItemText(3, QCoreApplication::translate("PGetFourierBlockDialog", "Strong", nullptr));

        fPlot_comboBox->setItemText(0, QCoreApplication::translate("PGetFourierBlockDialog", "Power", nullptr));
        fPlot_comboBox->setItemText(1, QCoreApplication::translate("PGetFourierBlockDialog", "Real", nullptr));
        fPlot_comboBox->setItemText(2, QCoreApplication::translate("PGetFourierBlockDialog", "Imag", nullptr));
        fPlot_comboBox->setItemText(3, QCoreApplication::translate("PGetFourierBlockDialog", "Real & Imag", nullptr));
        fPlot_comboBox->setItemText(4, QCoreApplication::translate("PGetFourierBlockDialog", "Phase", nullptr));

    } // retranslateUi

};

namespace Ui {
    class PGetFourierBlockDialog: public Ui_PGetFourierBlockDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PGETFOURIERBLOCKDIALOG_H
