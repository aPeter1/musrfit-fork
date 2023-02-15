/********************************************************************************
** Form generated from reading UI file 'PReplaceConfirmationDialog.ui'
**
** Created by: Qt User Interface Compiler version 6.4.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PREPLACECONFIRMATIONDIALOG_H
#define UI_PREPLACECONFIRMATIONDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFrame>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_PReplaceConfirmationDialog
{
public:
    QLabel *fTextLabel;
    QFrame *line;
    QPushButton *fReplace_pushButton;
    QPushButton *fReplaceAndClose_pushButton;
    QPushButton *fReplaceAll_pushButton;
    QPushButton *fFindNext_pushButton;
    QPushButton *fClose_pushButton;

    void setupUi(QDialog *PReplaceConfirmationDialog)
    {
        if (PReplaceConfirmationDialog->objectName().isEmpty())
            PReplaceConfirmationDialog->setObjectName("PReplaceConfirmationDialog");
        PReplaceConfirmationDialog->setWindowModality(Qt::WindowModal);
        PReplaceConfirmationDialog->resize(513, 65);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/images/musrfit.xpm"), QSize(), QIcon::Normal, QIcon::Off);
        PReplaceConfirmationDialog->setWindowIcon(icon);
        fTextLabel = new QLabel(PReplaceConfirmationDialog);
        fTextLabel->setObjectName("fTextLabel");
        fTextLabel->setGeometry(QRect(10, 10, 461, 16));
        line = new QFrame(PReplaceConfirmationDialog);
        line->setObjectName("line");
        line->setGeometry(QRect(7, 20, 521, 20));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        fReplace_pushButton = new QPushButton(PReplaceConfirmationDialog);
        fReplace_pushButton->setObjectName("fReplace_pushButton");
        fReplace_pushButton->setGeometry(QRect(7, 35, 81, 25));
        fReplaceAndClose_pushButton = new QPushButton(PReplaceConfirmationDialog);
        fReplaceAndClose_pushButton->setObjectName("fReplaceAndClose_pushButton");
        fReplaceAndClose_pushButton->setGeometry(QRect(87, 35, 131, 25));
        fReplaceAll_pushButton = new QPushButton(PReplaceConfirmationDialog);
        fReplaceAll_pushButton->setObjectName("fReplaceAll_pushButton");
        fReplaceAll_pushButton->setGeometry(QRect(217, 35, 97, 25));
        fFindNext_pushButton = new QPushButton(PReplaceConfirmationDialog);
        fFindNext_pushButton->setObjectName("fFindNext_pushButton");
        fFindNext_pushButton->setGeometry(QRect(317, 35, 91, 25));
        fClose_pushButton = new QPushButton(PReplaceConfirmationDialog);
        fClose_pushButton->setObjectName("fClose_pushButton");
        fClose_pushButton->setGeometry(QRect(407, 35, 97, 25));

        retranslateUi(PReplaceConfirmationDialog);
        QObject::connect(fClose_pushButton, &QPushButton::clicked, PReplaceConfirmationDialog, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(PReplaceConfirmationDialog);
    } // setupUi

    void retranslateUi(QDialog *PReplaceConfirmationDialog)
    {
        PReplaceConfirmationDialog->setWindowTitle(QCoreApplication::translate("PReplaceConfirmationDialog", "Replace Confirmation - musredit", nullptr));
        fTextLabel->setText(QCoreApplication::translate("PReplaceConfirmationDialog", "Found an occurrence of your search term.What do you want to do?", nullptr));
        fReplace_pushButton->setText(QCoreApplication::translate("PReplaceConfirmationDialog", "&Replace", nullptr));
        fReplaceAndClose_pushButton->setText(QCoreApplication::translate("PReplaceConfirmationDialog", "Re&place and Close", nullptr));
        fReplaceAll_pushButton->setText(QCoreApplication::translate("PReplaceConfirmationDialog", "Replace &All", nullptr));
        fFindNext_pushButton->setText(QCoreApplication::translate("PReplaceConfirmationDialog", "&Find Next", nullptr));
        fClose_pushButton->setText(QCoreApplication::translate("PReplaceConfirmationDialog", "&Close", nullptr));
    } // retranslateUi

};

namespace Ui {
    class PReplaceConfirmationDialog: public Ui_PReplaceConfirmationDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PREPLACECONFIRMATIONDIALOG_H
