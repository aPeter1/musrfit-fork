/********************************************************************************
** Form generated from reading UI file 'PGetTitleBlockDialog.ui'
**
** Created by: Qt User Interface Compiler version 6.4.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PGETTITLEBLOCKDIALOG_H
#define UI_PGETTITLEBLOCKDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PGetTitleBlockDialog
{
public:
    QLabel *fTitle_label;
    QLineEdit *fTitle_lineEdit;
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout;
    QFrame *line;
    QHBoxLayout *horizontalLayout;
    QPushButton *fHelp_pushButton;
    QSpacerItem *horizontalSpacer;
    QPushButton *fOk_pushButton;
    QPushButton *fCancel_pushButton;

    void setupUi(QDialog *PGetTitleBlockDialog)
    {
        if (PGetTitleBlockDialog->objectName().isEmpty())
            PGetTitleBlockDialog->setObjectName("PGetTitleBlockDialog");
        PGetTitleBlockDialog->setWindowModality(Qt::WindowModal);
        PGetTitleBlockDialog->resize(556, 90);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/images/musrfit.xpm"), QSize(), QIcon::Normal, QIcon::Off);
        PGetTitleBlockDialog->setWindowIcon(icon);
        fTitle_label = new QLabel(PGetTitleBlockDialog);
        fTitle_label->setObjectName("fTitle_label");
        fTitle_label->setGeometry(QRect(10, 10, 31, 16));
        fTitle_lineEdit = new QLineEdit(PGetTitleBlockDialog);
        fTitle_lineEdit->setObjectName("fTitle_lineEdit");
        fTitle_lineEdit->setGeometry(QRect(50, 6, 501, 26));
        layoutWidget = new QWidget(PGetTitleBlockDialog);
        layoutWidget->setObjectName("layoutWidget");
        layoutWidget->setGeometry(QRect(10, 40, 541, 47));
        verticalLayout = new QVBoxLayout(layoutWidget);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        line = new QFrame(layoutWidget);
        line->setObjectName("line");
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        verticalLayout->addWidget(line);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        fHelp_pushButton = new QPushButton(layoutWidget);
        fHelp_pushButton->setObjectName("fHelp_pushButton");

        horizontalLayout->addWidget(fHelp_pushButton);

        horizontalSpacer = new QSpacerItem(138, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        fOk_pushButton = new QPushButton(layoutWidget);
        fOk_pushButton->setObjectName("fOk_pushButton");

        horizontalLayout->addWidget(fOk_pushButton);

        fCancel_pushButton = new QPushButton(layoutWidget);
        fCancel_pushButton->setObjectName("fCancel_pushButton");

        horizontalLayout->addWidget(fCancel_pushButton);


        verticalLayout->addLayout(horizontalLayout);

        QWidget::setTabOrder(fTitle_lineEdit, fOk_pushButton);
        QWidget::setTabOrder(fOk_pushButton, fCancel_pushButton);
        QWidget::setTabOrder(fCancel_pushButton, fHelp_pushButton);

        retranslateUi(PGetTitleBlockDialog);
        QObject::connect(fOk_pushButton, &QPushButton::clicked, PGetTitleBlockDialog, qOverload<>(&QDialog::accept));
        QObject::connect(fCancel_pushButton, &QPushButton::clicked, PGetTitleBlockDialog, qOverload<>(&QDialog::reject));
        QObject::connect(fHelp_pushButton, SIGNAL(clicked()), PGetTitleBlockDialog, SLOT(helpContent()));

        QMetaObject::connectSlotsByName(PGetTitleBlockDialog);
    } // setupUi

    void retranslateUi(QDialog *PGetTitleBlockDialog)
    {
        PGetTitleBlockDialog->setWindowTitle(QCoreApplication::translate("PGetTitleBlockDialog", "Get Title", nullptr));
        fTitle_label->setText(QCoreApplication::translate("PGetTitleBlockDialog", "Title", nullptr));
        fHelp_pushButton->setText(QCoreApplication::translate("PGetTitleBlockDialog", "&Help", nullptr));
        fOk_pushButton->setText(QCoreApplication::translate("PGetTitleBlockDialog", "&OK", nullptr));
        fCancel_pushButton->setText(QCoreApplication::translate("PGetTitleBlockDialog", "&Cancel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class PGetTitleBlockDialog: public Ui_PGetTitleBlockDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PGETTITLEBLOCKDIALOG_H
