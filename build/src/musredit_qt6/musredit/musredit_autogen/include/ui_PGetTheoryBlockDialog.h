/********************************************************************************
** Form generated from reading UI file 'PGetTheoryBlockDialog.ui'
**
** Created by: Qt User Interface Compiler version 6.4.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PGETTHEORYBLOCKDIALOG_H
#define UI_PGETTHEORYBLOCKDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PGetTheoryBlockDialog
{
public:
    QTextEdit *fDescription_textEdit;
    QGroupBox *fTheoryInput_groupBox;
    QComboBox *fTheoryFunction_comboBox;
    QPushButton *fPlus_pushButton;
    QPushButton *fMultiply_pushButton;
    QPlainTextEdit *fTheoryBlock_plainTextEdit;
    QWidget *layoutWidget;
    QHBoxLayout *horizontalLayout;
    QPushButton *fHelp_pushButton;
    QSpacerItem *horizontalSpacer;
    QPushButton *fOk_pushButton;
    QPushButton *fCancel_pushButton;

    void setupUi(QDialog *PGetTheoryBlockDialog)
    {
        if (PGetTheoryBlockDialog->objectName().isEmpty())
            PGetTheoryBlockDialog->setObjectName("PGetTheoryBlockDialog");
        PGetTheoryBlockDialog->setWindowModality(Qt::WindowModal);
        PGetTheoryBlockDialog->resize(698, 546);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/images/musrfit.xpm"), QSize(), QIcon::Normal, QIcon::Off);
        PGetTheoryBlockDialog->setWindowIcon(icon);
        fDescription_textEdit = new QTextEdit(PGetTheoryBlockDialog);
        fDescription_textEdit->setObjectName("fDescription_textEdit");
        fDescription_textEdit->setGeometry(QRect(10, 10, 681, 211));
        fDescription_textEdit->setReadOnly(true);
        fTheoryInput_groupBox = new QGroupBox(PGetTheoryBlockDialog);
        fTheoryInput_groupBox->setObjectName("fTheoryInput_groupBox");
        fTheoryInput_groupBox->setGeometry(QRect(10, 220, 681, 71));
        fTheoryFunction_comboBox = new QComboBox(fTheoryInput_groupBox);
        fTheoryFunction_comboBox->setObjectName("fTheoryFunction_comboBox");
        fTheoryFunction_comboBox->setGeometry(QRect(10, 20, 541, 41));
        fPlus_pushButton = new QPushButton(fTheoryInput_groupBox);
        fPlus_pushButton->setObjectName("fPlus_pushButton");
        fPlus_pushButton->setGeometry(QRect(560, 30, 51, 25));
        fMultiply_pushButton = new QPushButton(fTheoryInput_groupBox);
        fMultiply_pushButton->setObjectName("fMultiply_pushButton");
        fMultiply_pushButton->setGeometry(QRect(620, 30, 51, 25));
        fTheoryBlock_plainTextEdit = new QPlainTextEdit(PGetTheoryBlockDialog);
        fTheoryBlock_plainTextEdit->setObjectName("fTheoryBlock_plainTextEdit");
        fTheoryBlock_plainTextEdit->setGeometry(QRect(10, 300, 681, 201));
        layoutWidget = new QWidget(PGetTheoryBlockDialog);
        layoutWidget->setObjectName("layoutWidget");
        layoutWidget->setGeometry(QRect(10, 510, 681, 29));
        horizontalLayout = new QHBoxLayout(layoutWidget);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        fHelp_pushButton = new QPushButton(layoutWidget);
        fHelp_pushButton->setObjectName("fHelp_pushButton");

        horizontalLayout->addWidget(fHelp_pushButton);

        horizontalSpacer = new QSpacerItem(178, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        fOk_pushButton = new QPushButton(layoutWidget);
        fOk_pushButton->setObjectName("fOk_pushButton");

        horizontalLayout->addWidget(fOk_pushButton);

        fCancel_pushButton = new QPushButton(layoutWidget);
        fCancel_pushButton->setObjectName("fCancel_pushButton");

        horizontalLayout->addWidget(fCancel_pushButton);

        QWidget::setTabOrder(fTheoryFunction_comboBox, fPlus_pushButton);
        QWidget::setTabOrder(fPlus_pushButton, fMultiply_pushButton);
        QWidget::setTabOrder(fMultiply_pushButton, fTheoryBlock_plainTextEdit);
        QWidget::setTabOrder(fTheoryBlock_plainTextEdit, fOk_pushButton);
        QWidget::setTabOrder(fOk_pushButton, fCancel_pushButton);
        QWidget::setTabOrder(fCancel_pushButton, fHelp_pushButton);
        QWidget::setTabOrder(fHelp_pushButton, fDescription_textEdit);

        retranslateUi(PGetTheoryBlockDialog);
        QObject::connect(fCancel_pushButton, &QPushButton::clicked, PGetTheoryBlockDialog, qOverload<>(&QDialog::reject));
        QObject::connect(fOk_pushButton, &QPushButton::clicked, PGetTheoryBlockDialog, qOverload<>(&QDialog::accept));
        QObject::connect(fHelp_pushButton, SIGNAL(clicked()), PGetTheoryBlockDialog, SLOT(helpContent()));
        QObject::connect(fPlus_pushButton, SIGNAL(clicked()), PGetTheoryBlockDialog, SLOT(addPlus()));
        QObject::connect(fMultiply_pushButton, SIGNAL(clicked()), PGetTheoryBlockDialog, SLOT(addMultiply()));

        QMetaObject::connectSlotsByName(PGetTheoryBlockDialog);
    } // setupUi

    void retranslateUi(QDialog *PGetTheoryBlockDialog)
    {
        PGetTheoryBlockDialog->setWindowTitle(QCoreApplication::translate("PGetTheoryBlockDialog", "Get Theory", nullptr));
        fDescription_textEdit->setHtml(QCoreApplication::translate("PGetTheoryBlockDialog", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
"<table border=\"0\" style=\"-qt-table-type: root; margin-top:4px; margin-bottom:4px; margin-left:4px; margin-right:4px;\">\n"
"<tr>\n"
"<td style=\"border: none;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Every theory function has to be written on a single line. It starts with the theory function name or its abbreviation followed by the parameters. Consecutive lines of theory functions will be multiplied. If theory functions need to be added, a line with a '+' has to separate them. The parameters are given as the numbers assigned to them in the FITPARAMETER-block.</p>\n"
"<p style=\"-qt-paragraph"
                        "-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Example:</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">asymmetry 2</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">simplExpo 3</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">TFieldCos map1 fun2</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">+</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">asymmetry 6</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin"
                        "-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">simplExpo 7</p></td></tr></table></body></html>", nullptr));
        fTheoryInput_groupBox->setTitle(QCoreApplication::translate("PGetTheoryBlockDialog", "Theory Input Line", nullptr));
        fPlus_pushButton->setText(QCoreApplication::translate("PGetTheoryBlockDialog", "+", nullptr));
        fMultiply_pushButton->setText(QCoreApplication::translate("PGetTheoryBlockDialog", "*", nullptr));
        fTheoryBlock_plainTextEdit->setPlainText(QCoreApplication::translate("PGetTheoryBlockDialog", "#####################################################\n"
"THEORY", nullptr));
        fHelp_pushButton->setText(QCoreApplication::translate("PGetTheoryBlockDialog", "&Help", nullptr));
        fOk_pushButton->setText(QCoreApplication::translate("PGetTheoryBlockDialog", "&OK", nullptr));
        fCancel_pushButton->setText(QCoreApplication::translate("PGetTheoryBlockDialog", "&Cancel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class PGetTheoryBlockDialog: public Ui_PGetTheoryBlockDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PGETTHEORYBLOCKDIALOG_H
