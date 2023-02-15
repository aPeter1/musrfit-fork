/********************************************************************************
** Form generated from reading UI file 'PMusrEditAbout.ui'
**
** Created by: Qt User Interface Compiler version 6.4.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PMUSREDITABOUT_H
#define UI_PMUSREDITABOUT_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PMusrEditAbout
{
public:
    QWidget *widget;
    QVBoxLayout *verticalLayout;
    QLabel *fMusrEditAbout_label;
    QLabel *fMusrfitVersion_label;
    QLabel *fGitBranch_label;
    QLabel *fGitRev_label;
    QLabel *fRootVersion_label;
    QLabel *fDetails_label;
    QPushButton *fOk_pushButton;

    void setupUi(QDialog *PMusrEditAbout)
    {
        if (PMusrEditAbout->objectName().isEmpty())
            PMusrEditAbout->setObjectName("PMusrEditAbout");
        PMusrEditAbout->resize(395, 331);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/images/musrfit.xpm"), QSize(), QIcon::Normal, QIcon::Off);
        PMusrEditAbout->setWindowIcon(icon);
        widget = new QWidget(PMusrEditAbout);
        widget->setObjectName("widget");
        widget->setGeometry(QRect(14, 14, 371, 311));
        verticalLayout = new QVBoxLayout(widget);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        fMusrEditAbout_label = new QLabel(widget);
        fMusrEditAbout_label->setObjectName("fMusrEditAbout_label");
        QFont font;
        font.setPointSize(24);
        font.setBold(false);
        font.setItalic(true);
        fMusrEditAbout_label->setFont(font);
        fMusrEditAbout_label->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(fMusrEditAbout_label);

        fMusrfitVersion_label = new QLabel(widget);
        fMusrfitVersion_label->setObjectName("fMusrfitVersion_label");
        QFont font1;
        font1.setBold(true);
        font1.setItalic(true);
        fMusrfitVersion_label->setFont(font1);
        fMusrfitVersion_label->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(fMusrfitVersion_label);

        fGitBranch_label = new QLabel(widget);
        fGitBranch_label->setObjectName("fGitBranch_label");
        fGitBranch_label->setFont(font1);
        fGitBranch_label->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(fGitBranch_label);

        fGitRev_label = new QLabel(widget);
        fGitRev_label->setObjectName("fGitRev_label");
        fGitRev_label->setFont(font1);
        fGitRev_label->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(fGitRev_label);

        fRootVersion_label = new QLabel(widget);
        fRootVersion_label->setObjectName("fRootVersion_label");
        fRootVersion_label->setFont(font1);
        fRootVersion_label->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(fRootVersion_label);

        fDetails_label = new QLabel(widget);
        fDetails_label->setObjectName("fDetails_label");

        verticalLayout->addWidget(fDetails_label);

        fOk_pushButton = new QPushButton(widget);
        fOk_pushButton->setObjectName("fOk_pushButton");

        verticalLayout->addWidget(fOk_pushButton);


        retranslateUi(PMusrEditAbout);
        QObject::connect(fOk_pushButton, &QPushButton::clicked, PMusrEditAbout, qOverload<>(&QDialog::accept));

        QMetaObject::connectSlotsByName(PMusrEditAbout);
    } // setupUi

    void retranslateUi(QDialog *PMusrEditAbout)
    {
        PMusrEditAbout->setWindowTitle(QCoreApplication::translate("PMusrEditAbout", "Musr Edit About", nullptr));
        fMusrEditAbout_label->setText(QCoreApplication::translate("PMusrEditAbout", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:24pt; font-weight:400; font-style:italic;\">\n"
"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">musredit ...</p></body></html>", nullptr));
        fMusrfitVersion_label->setText(QCoreApplication::translate("PMusrEditAbout", "musrfit-version:", nullptr));
        fGitBranch_label->setText(QCoreApplication::translate("PMusrEditAbout", "<html><head/><body><p><span style=\" font-weight:600; font-style:italic;\">git-branch: </span></p></body></html>", nullptr));
        fGitRev_label->setText(QCoreApplication::translate("PMusrEditAbout", "git-rev:", nullptr));
        fRootVersion_label->setText(QCoreApplication::translate("PMusrEditAbout", "ROOT-version:", nullptr));
        fDetails_label->setText(QCoreApplication::translate("PMusrEditAbout", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:10pt; font-weight:400; font-style:normal;\">\n"
"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">by Andreas Suter</p>\n"
"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">andreas.suter@psi.ch</p>\n"
"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Paul Scherrer Institute</p>\n"
"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Switzerland</p></body></html>", nullptr));
        fOk_pushButton->setText(QCoreApplication::translate("PMusrEditAbout", "&OK", nullptr));
    } // retranslateUi

};

namespace Ui {
    class PMusrEditAbout: public Ui_PMusrEditAbout {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PMUSREDITABOUT_H
