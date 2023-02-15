/********************************************************************************
** Form generated from reading UI file 'PChangeDefaultPathsDialog.ui'
**
** Created by: Qt User Interface Compiler version 6.4.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PCHANGEDEFAULTPATHSDIALOG_H
#define UI_PCHANGEDEFAULTPATHSDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PChangeDefaultPathsDialog
{
public:
    QWidget *widget;
    QVBoxLayout *verticalLayout;
    QListWidget *fSearchPath_listWidget;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *fAdd_pushButton;
    QPushButton *fDelete_pushButton;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *PChangeDefaultPathsDialog)
    {
        if (PChangeDefaultPathsDialog->objectName().isEmpty())
            PChangeDefaultPathsDialog->setObjectName("PChangeDefaultPathsDialog");
        PChangeDefaultPathsDialog->resize(528, 363);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(PChangeDefaultPathsDialog->sizePolicy().hasHeightForWidth());
        PChangeDefaultPathsDialog->setSizePolicy(sizePolicy);
        widget = new QWidget(PChangeDefaultPathsDialog);
        widget->setObjectName("widget");
        widget->setGeometry(QRect(9, 10, 512, 341));
        verticalLayout = new QVBoxLayout(widget);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        fSearchPath_listWidget = new QListWidget(widget);
        fSearchPath_listWidget->setObjectName("fSearchPath_listWidget");

        verticalLayout->addWidget(fSearchPath_listWidget);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalSpacer = new QSpacerItem(328, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        fAdd_pushButton = new QPushButton(widget);
        fAdd_pushButton->setObjectName("fAdd_pushButton");

        horizontalLayout->addWidget(fAdd_pushButton);

        fDelete_pushButton = new QPushButton(widget);
        fDelete_pushButton->setObjectName("fDelete_pushButton");

        horizontalLayout->addWidget(fDelete_pushButton);


        verticalLayout->addLayout(horizontalLayout);

        buttonBox = new QDialogButtonBox(widget);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(PChangeDefaultPathsDialog);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, PChangeDefaultPathsDialog, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, PChangeDefaultPathsDialog, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(PChangeDefaultPathsDialog);
    } // setupUi

    void retranslateUi(QDialog *PChangeDefaultPathsDialog)
    {
        PChangeDefaultPathsDialog->setWindowTitle(QCoreApplication::translate("PChangeDefaultPathsDialog", "Dialog", nullptr));
        fAdd_pushButton->setText(QCoreApplication::translate("PChangeDefaultPathsDialog", "Add", nullptr));
        fDelete_pushButton->setText(QCoreApplication::translate("PChangeDefaultPathsDialog", "Delete", nullptr));
    } // retranslateUi

};

namespace Ui {
    class PChangeDefaultPathsDialog: public Ui_PChangeDefaultPathsDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PCHANGEDEFAULTPATHSDIALOG_H
