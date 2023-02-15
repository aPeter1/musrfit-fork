/********************************************************************************
** Form generated from reading UI file 'PGetParameterBlockDialog.ui'
**
** Created by: Qt User Interface Compiler version 6.4.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PGETPARAMETERBLOCKDIALOG_H
#define UI_PGETPARAMETERBLOCKDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PGetParameterBlockDialog
{
public:
    QWidget *layoutWidget;
    QHBoxLayout *horizontalLayout;
    QPushButton *fHelp_pushButton;
    QSpacerItem *horizontalSpacer;
    QPushButton *fOk_pushButton;
    QPushButton *fCancel_pushButton;
    QPlainTextEdit *fParam_plainTextEdit;
    QPushButton *fAdd_pushButton;
    QLabel *fParamNo_label;
    QLabel *fName_label;
    QLabel *fValue_label;
    QLabel *fStep_label;
    QGroupBox *fBoundaries_groupBox;
    QLabel *fLower_label;
    QLabel *fUpper_label;
    QLineEdit *fLower_lineEdit;
    QLineEdit *fUpper_lineEdit;
    QSpinBox *fParamNo_spinBox;
    QLineEdit *fName_lineEdit;
    QLineEdit *fValue_lineEdit;
    QLineEdit *fStep_lineEdit;

    void setupUi(QDialog *PGetParameterBlockDialog)
    {
        if (PGetParameterBlockDialog->objectName().isEmpty())
            PGetParameterBlockDialog->setObjectName("PGetParameterBlockDialog");
        PGetParameterBlockDialog->setWindowModality(Qt::WindowModal);
        PGetParameterBlockDialog->resize(656, 476);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/images/musrfit.xpm"), QSize(), QIcon::Normal, QIcon::Off);
        PGetParameterBlockDialog->setWindowIcon(icon);
        layoutWidget = new QWidget(PGetParameterBlockDialog);
        layoutWidget->setObjectName("layoutWidget");
        layoutWidget->setGeometry(QRect(10, 430, 631, 32));
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

        fParam_plainTextEdit = new QPlainTextEdit(PGetParameterBlockDialog);
        fParam_plainTextEdit->setObjectName("fParam_plainTextEdit");
        fParam_plainTextEdit->setGeometry(QRect(10, 90, 631, 331));
        fAdd_pushButton = new QPushButton(PGetParameterBlockDialog);
        fAdd_pushButton->setObjectName("fAdd_pushButton");
        fAdd_pushButton->setGeometry(QRect(560, 50, 75, 23));
        fParamNo_label = new QLabel(PGetParameterBlockDialog);
        fParamNo_label->setObjectName("fParamNo_label");
        fParamNo_label->setGeometry(QRect(10, 30, 31, 13));
        fName_label = new QLabel(PGetParameterBlockDialog);
        fName_label->setObjectName("fName_label");
        fName_label->setGeometry(QRect(60, 30, 46, 13));
        fValue_label = new QLabel(PGetParameterBlockDialog);
        fValue_label->setObjectName("fValue_label");
        fValue_label->setGeometry(QRect(180, 30, 46, 13));
        fStep_label = new QLabel(PGetParameterBlockDialog);
        fStep_label->setObjectName("fStep_label");
        fStep_label->setGeometry(QRect(270, 30, 46, 13));
        fBoundaries_groupBox = new QGroupBox(PGetParameterBlockDialog);
        fBoundaries_groupBox->setObjectName("fBoundaries_groupBox");
        fBoundaries_groupBox->setGeometry(QRect(370, 10, 171, 71));
        fLower_label = new QLabel(fBoundaries_groupBox);
        fLower_label->setObjectName("fLower_label");
        fLower_label->setGeometry(QRect(10, 20, 46, 13));
        fUpper_label = new QLabel(fBoundaries_groupBox);
        fUpper_label->setObjectName("fUpper_label");
        fUpper_label->setGeometry(QRect(90, 20, 46, 13));
        fLower_lineEdit = new QLineEdit(fBoundaries_groupBox);
        fLower_lineEdit->setObjectName("fLower_lineEdit");
        fLower_lineEdit->setGeometry(QRect(10, 40, 71, 20));
        fUpper_lineEdit = new QLineEdit(fBoundaries_groupBox);
        fUpper_lineEdit->setObjectName("fUpper_lineEdit");
        fUpper_lineEdit->setGeometry(QRect(90, 40, 71, 20));
        fParamNo_spinBox = new QSpinBox(PGetParameterBlockDialog);
        fParamNo_spinBox->setObjectName("fParamNo_spinBox");
        fParamNo_spinBox->setGeometry(QRect(10, 50, 42, 22));
        fParamNo_spinBox->setMinimum(1);
        fParamNo_spinBox->setMaximum(999);
        fName_lineEdit = new QLineEdit(PGetParameterBlockDialog);
        fName_lineEdit->setObjectName("fName_lineEdit");
        fName_lineEdit->setGeometry(QRect(60, 50, 113, 20));
        fValue_lineEdit = new QLineEdit(PGetParameterBlockDialog);
        fValue_lineEdit->setObjectName("fValue_lineEdit");
        fValue_lineEdit->setGeometry(QRect(180, 50, 81, 20));
        fStep_lineEdit = new QLineEdit(PGetParameterBlockDialog);
        fStep_lineEdit->setObjectName("fStep_lineEdit");
        fStep_lineEdit->setGeometry(QRect(270, 50, 81, 20));
        QWidget::setTabOrder(fParamNo_spinBox, fName_lineEdit);
        QWidget::setTabOrder(fName_lineEdit, fValue_lineEdit);
        QWidget::setTabOrder(fValue_lineEdit, fStep_lineEdit);
        QWidget::setTabOrder(fStep_lineEdit, fLower_lineEdit);
        QWidget::setTabOrder(fLower_lineEdit, fUpper_lineEdit);
        QWidget::setTabOrder(fUpper_lineEdit, fAdd_pushButton);
        QWidget::setTabOrder(fAdd_pushButton, fOk_pushButton);
        QWidget::setTabOrder(fOk_pushButton, fCancel_pushButton);
        QWidget::setTabOrder(fCancel_pushButton, fParam_plainTextEdit);
        QWidget::setTabOrder(fParam_plainTextEdit, fHelp_pushButton);

        retranslateUi(PGetParameterBlockDialog);
        QObject::connect(fCancel_pushButton, &QPushButton::clicked, PGetParameterBlockDialog, qOverload<>(&QDialog::reject));
        QObject::connect(fOk_pushButton, &QPushButton::clicked, PGetParameterBlockDialog, qOverload<>(&QDialog::accept));
        QObject::connect(fAdd_pushButton, SIGNAL(clicked()), PGetParameterBlockDialog, SLOT(paramAdd()));
        QObject::connect(fHelp_pushButton, SIGNAL(clicked()), PGetParameterBlockDialog, SLOT(helpContent()));

        QMetaObject::connectSlotsByName(PGetParameterBlockDialog);
    } // setupUi

    void retranslateUi(QDialog *PGetParameterBlockDialog)
    {
        PGetParameterBlockDialog->setWindowTitle(QCoreApplication::translate("PGetParameterBlockDialog", "Get Parameter", nullptr));
        fHelp_pushButton->setText(QCoreApplication::translate("PGetParameterBlockDialog", "&Help", nullptr));
        fOk_pushButton->setText(QCoreApplication::translate("PGetParameterBlockDialog", "&OK", nullptr));
        fCancel_pushButton->setText(QCoreApplication::translate("PGetParameterBlockDialog", "&Cancel", nullptr));
        fParam_plainTextEdit->setPlainText(QCoreApplication::translate("PGetParameterBlockDialog", "###############################################################\n"
"FITPARAMETER\n"
"#      Nr. Name        Value     Step      Pos_Error  Boundaries", nullptr));
        fAdd_pushButton->setText(QCoreApplication::translate("PGetParameterBlockDialog", "&Add", nullptr));
        fParamNo_label->setText(QCoreApplication::translate("PGetParameterBlockDialog", "No.", nullptr));
        fName_label->setText(QCoreApplication::translate("PGetParameterBlockDialog", "Name", nullptr));
        fValue_label->setText(QCoreApplication::translate("PGetParameterBlockDialog", "Value", nullptr));
        fStep_label->setText(QCoreApplication::translate("PGetParameterBlockDialog", "Step", nullptr));
        fBoundaries_groupBox->setTitle(QCoreApplication::translate("PGetParameterBlockDialog", "Boundaries", nullptr));
        fLower_label->setText(QCoreApplication::translate("PGetParameterBlockDialog", "Lower", nullptr));
        fUpper_label->setText(QCoreApplication::translate("PGetParameterBlockDialog", "Upper", nullptr));
        fLower_lineEdit->setText(QCoreApplication::translate("PGetParameterBlockDialog", "none", nullptr));
        fUpper_lineEdit->setText(QCoreApplication::translate("PGetParameterBlockDialog", "none", nullptr));
    } // retranslateUi

};

namespace Ui {
    class PGetParameterBlockDialog: public Ui_PGetParameterBlockDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PGETPARAMETERBLOCKDIALOG_H
