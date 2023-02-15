/********************************************************************************
** Form generated from reading UI file 'PGetFunctionsBlockDialog.ui'
**
** Created by: Qt User Interface Compiler version 6.4.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PGETFUNCTIONSBLOCKDIALOG_H
#define UI_PGETFUNCTIONSBLOCKDIALOG_H

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
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PGetFunctionsBlockDialog
{
public:
    QWidget *layoutWidget;
    QHBoxLayout *horizontalLayout;
    QPushButton *fHelp_pushButton;
    QSpacerItem *horizontalSpacer;
    QPushButton *fOk_pushButton;
    QPushButton *fCancel_pushButton;
    QTextEdit *fHelp_textEdit;
    QGroupBox *fInput_groupBox;
    QLineEdit *fFunctionInput_lineEdit;
    QLabel *fFunction_label;
    QPushButton *fAdd_pushButton;
    QPlainTextEdit *fFunctionBlock_plainTextEdit;

    void setupUi(QDialog *PGetFunctionsBlockDialog)
    {
        if (PGetFunctionsBlockDialog->objectName().isEmpty())
            PGetFunctionsBlockDialog->setObjectName("PGetFunctionsBlockDialog");
        PGetFunctionsBlockDialog->setWindowModality(Qt::WindowModal);
        PGetFunctionsBlockDialog->resize(636, 604);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/images/musrfit.xpm"), QSize(), QIcon::Normal, QIcon::Off);
        PGetFunctionsBlockDialog->setWindowIcon(icon);
        layoutWidget = new QWidget(PGetFunctionsBlockDialog);
        layoutWidget->setObjectName("layoutWidget");
        layoutWidget->setGeometry(QRect(0, 570, 631, 32));
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

        fHelp_textEdit = new QTextEdit(PGetFunctionsBlockDialog);
        fHelp_textEdit->setObjectName("fHelp_textEdit");
        fHelp_textEdit->setGeometry(QRect(0, 10, 631, 201));
        fHelp_textEdit->setReadOnly(true);
        fInput_groupBox = new QGroupBox(PGetFunctionsBlockDialog);
        fInput_groupBox->setObjectName("fInput_groupBox");
        fInput_groupBox->setGeometry(QRect(0, 210, 631, 101));
        fFunctionInput_lineEdit = new QLineEdit(fInput_groupBox);
        fFunctionInput_lineEdit->setObjectName("fFunctionInput_lineEdit");
        fFunctionInput_lineEdit->setGeometry(QRect(90, 30, 521, 22));
        fFunction_label = new QLabel(fInput_groupBox);
        fFunction_label->setObjectName("fFunction_label");
        fFunction_label->setGeometry(QRect(20, 30, 71, 17));
        fAdd_pushButton = new QPushButton(fInput_groupBox);
        fAdd_pushButton->setObjectName("fAdd_pushButton");
        fAdd_pushButton->setGeometry(QRect(500, 60, 113, 32));
        fFunctionBlock_plainTextEdit = new QPlainTextEdit(PGetFunctionsBlockDialog);
        fFunctionBlock_plainTextEdit->setObjectName("fFunctionBlock_plainTextEdit");
        fFunctionBlock_plainTextEdit->setGeometry(QRect(0, 310, 631, 251));
        QWidget::setTabOrder(fOk_pushButton, fCancel_pushButton);
        QWidget::setTabOrder(fCancel_pushButton, fHelp_pushButton);

        retranslateUi(PGetFunctionsBlockDialog);
        QObject::connect(fCancel_pushButton, &QPushButton::clicked, PGetFunctionsBlockDialog, qOverload<>(&QDialog::reject));
        QObject::connect(fOk_pushButton, &QPushButton::clicked, PGetFunctionsBlockDialog, qOverload<>(&QDialog::accept));
        QObject::connect(fHelp_pushButton, SIGNAL(clicked()), PGetFunctionsBlockDialog, SLOT(helpContent()));
        QObject::connect(fAdd_pushButton, SIGNAL(clicked()), PGetFunctionsBlockDialog, SLOT(addFunction()));
        QObject::connect(fFunctionInput_lineEdit, SIGNAL(returnPressed()), PGetFunctionsBlockDialog, SLOT(addFunction()));

        QMetaObject::connectSlotsByName(PGetFunctionsBlockDialog);
    } // setupUi

    void retranslateUi(QDialog *PGetFunctionsBlockDialog)
    {
        PGetFunctionsBlockDialog->setWindowTitle(QCoreApplication::translate("PGetFunctionsBlockDialog", "Get Functions", nullptr));
        fHelp_pushButton->setText(QCoreApplication::translate("PGetFunctionsBlockDialog", "&Help", nullptr));
        fOk_pushButton->setText(QCoreApplication::translate("PGetFunctionsBlockDialog", "&OK", nullptr));
        fCancel_pushButton->setText(QCoreApplication::translate("PGetFunctionsBlockDialog", "&Cancel", nullptr));
        fHelp_textEdit->setHtml(QCoreApplication::translate("PGetFunctionsBlockDialog", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
"<table style=\"-qt-table-type: root; margin-top:4px; margin-bottom:4px; margin-left:4px; margin-right:4px;\">\n"
"<tr>\n"
"<td style=\"border: none;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'Lucida Grande'; font-size:10pt;\">Supported basic arithmetics: </span><span style=\" font-family:'Courier New,courier'; font-size:10pt;\">+, -, *, /, ()</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'Lucida Grande'; font-size:10pt;\">Supported built-in functions; </"
                        "span><span style=\" font-family:'Courier New,courier'; font-size:10pt;\">cos(), sin(), tan(), acos(), asin(), atan(), cosh(), sinh(), tanh(), acosh(), asinh(), atanh(), exp(), log(), ln()</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'Lucida Grande'; font-size:10pt;\">Supported pre-defined constants</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'Courier New,courier'; font-size:10pt;\">gamma_mu = 0.0135538817 (MHz/G)</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'Courier New,courier'; font-size:10pt;\">pi = 3.1415926535897932846</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><spa"
                        "n style=\" font-family:'Lucida Grande'; font-size:10pt;\">Parameters and Maps are reached via parX, mapY, where X, Y are numbers, e.g. par1, map3</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'Lucida Grande'; font-size:10pt;\">Examples:</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'Courier New,courier'; font-size:10pt;\">fun1 = gamma_mu * par3</span><span style=\" font-family:'Lucida Grande'; font-size:10pt;\"> valid</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'Courier New,courier'; font-size:10pt;\">fun2 = par2/map4 * sin(par3*par5)</span><span style=\" font-family:'Lucida Grande'; font-size:10pt;\"> valid</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom"
                        ":0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'Courier New,courier'; font-size:10pt;\">fun3 = fun1 + par6</span><span style=\" font-family:'Lucida Grande'; font-size:10pt;\"> invalid, since functions cannot be used with the functions definition</span></p></td></tr></table></body></html>", nullptr));
        fInput_groupBox->setTitle(QCoreApplication::translate("PGetFunctionsBlockDialog", "Input", nullptr));
        fFunction_label->setText(QCoreApplication::translate("PGetFunctionsBlockDialog", "Function:", nullptr));
        fAdd_pushButton->setText(QCoreApplication::translate("PGetFunctionsBlockDialog", "Add", nullptr));
        fFunctionBlock_plainTextEdit->setPlainText(QCoreApplication::translate("PGetFunctionsBlockDialog", "#####################################################\n"
"FUNCTIONS", nullptr));
    } // retranslateUi

};

namespace Ui {
    class PGetFunctionsBlockDialog: public Ui_PGetFunctionsBlockDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PGETFUNCTIONSBLOCKDIALOG_H
