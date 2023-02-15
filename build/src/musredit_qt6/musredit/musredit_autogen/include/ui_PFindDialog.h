/********************************************************************************
** Form generated from reading UI file 'PFindDialog.ui'
**
** Created by: Qt User Interface Compiler version 6.4.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PFINDDIALOG_H
#define UI_PFINDDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PFindDialog
{
public:
    QWidget *widget;
    QVBoxLayout *verticalLayout_2;
    QGroupBox *fFind_groupBox;
    QFormLayout *formLayout_2;
    QVBoxLayout *verticalLayout;
    QLabel *fFind_label;
    QComboBox *fFind_comboBox;
    QGroupBox *fOptions_groupBox;
    QFormLayout *formLayout;
    QGridLayout *gridLayout;
    QCheckBox *fCaseSensitive_checkBox;
    QCheckBox *fFindBackwards_checkBox;
    QCheckBox *fWholeWordsOnly_checkBox;
    QCheckBox *fSelectedText_checkBox;
    QCheckBox *fFromCursor_checkBox;
    QFrame *line;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *fFind_pushButton;
    QPushButton *fClose_pushButton;

    void setupUi(QDialog *PFindDialog)
    {
        if (PFindDialog->objectName().isEmpty())
            PFindDialog->setObjectName("PFindDialog");
        PFindDialog->resize(349, 271);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/images/musrfit.xpm"), QSize(), QIcon::Normal, QIcon::Off);
        PFindDialog->setWindowIcon(icon);
        widget = new QWidget(PFindDialog);
        widget->setObjectName("widget");
        widget->setGeometry(QRect(0, 0, 342, 264));
        verticalLayout_2 = new QVBoxLayout(widget);
        verticalLayout_2->setObjectName("verticalLayout_2");
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        fFind_groupBox = new QGroupBox(widget);
        fFind_groupBox->setObjectName("fFind_groupBox");
        formLayout_2 = new QFormLayout(fFind_groupBox);
        formLayout_2->setObjectName("formLayout_2");
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName("verticalLayout");
        fFind_label = new QLabel(fFind_groupBox);
        fFind_label->setObjectName("fFind_label");

        verticalLayout->addWidget(fFind_label);

        fFind_comboBox = new QComboBox(fFind_groupBox);
        fFind_comboBox->setObjectName("fFind_comboBox");
        fFind_comboBox->setEditable(true);

        verticalLayout->addWidget(fFind_comboBox);


        formLayout_2->setLayout(0, QFormLayout::SpanningRole, verticalLayout);


        verticalLayout_2->addWidget(fFind_groupBox);

        fOptions_groupBox = new QGroupBox(widget);
        fOptions_groupBox->setObjectName("fOptions_groupBox");
        fOptions_groupBox->setAlignment(Qt::AlignBottom|Qt::AlignLeading|Qt::AlignLeft);
        formLayout = new QFormLayout(fOptions_groupBox);
        formLayout->setObjectName("formLayout");
        gridLayout = new QGridLayout();
        gridLayout->setObjectName("gridLayout");
        fCaseSensitive_checkBox = new QCheckBox(fOptions_groupBox);
        fCaseSensitive_checkBox->setObjectName("fCaseSensitive_checkBox");

        gridLayout->addWidget(fCaseSensitive_checkBox, 0, 0, 1, 1);

        fFindBackwards_checkBox = new QCheckBox(fOptions_groupBox);
        fFindBackwards_checkBox->setObjectName("fFindBackwards_checkBox");

        gridLayout->addWidget(fFindBackwards_checkBox, 0, 1, 1, 1);

        fWholeWordsOnly_checkBox = new QCheckBox(fOptions_groupBox);
        fWholeWordsOnly_checkBox->setObjectName("fWholeWordsOnly_checkBox");

        gridLayout->addWidget(fWholeWordsOnly_checkBox, 1, 0, 1, 1);

        fSelectedText_checkBox = new QCheckBox(fOptions_groupBox);
        fSelectedText_checkBox->setObjectName("fSelectedText_checkBox");

        gridLayout->addWidget(fSelectedText_checkBox, 1, 1, 1, 1);

        fFromCursor_checkBox = new QCheckBox(fOptions_groupBox);
        fFromCursor_checkBox->setObjectName("fFromCursor_checkBox");

        gridLayout->addWidget(fFromCursor_checkBox, 2, 0, 1, 1);


        formLayout->setLayout(0, QFormLayout::SpanningRole, gridLayout);


        verticalLayout_2->addWidget(fOptions_groupBox);

        line = new QFrame(widget);
        line->setObjectName("line");
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        verticalLayout_2->addWidget(line);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalSpacer = new QSpacerItem(168, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        fFind_pushButton = new QPushButton(widget);
        fFind_pushButton->setObjectName("fFind_pushButton");

        horizontalLayout->addWidget(fFind_pushButton);

        fClose_pushButton = new QPushButton(widget);
        fClose_pushButton->setObjectName("fClose_pushButton");

        horizontalLayout->addWidget(fClose_pushButton);


        verticalLayout_2->addLayout(horizontalLayout);

        QWidget::setTabOrder(fFind_comboBox, fCaseSensitive_checkBox);
        QWidget::setTabOrder(fCaseSensitive_checkBox, fWholeWordsOnly_checkBox);
        QWidget::setTabOrder(fWholeWordsOnly_checkBox, fFromCursor_checkBox);
        QWidget::setTabOrder(fFromCursor_checkBox, fFindBackwards_checkBox);
        QWidget::setTabOrder(fFindBackwards_checkBox, fSelectedText_checkBox);
        QWidget::setTabOrder(fSelectedText_checkBox, fFind_pushButton);
        QWidget::setTabOrder(fFind_pushButton, fClose_pushButton);

        retranslateUi(PFindDialog);
        QObject::connect(fClose_pushButton, &QPushButton::clicked, PFindDialog, qOverload<>(&QDialog::reject));
        QObject::connect(fFind_pushButton, &QPushButton::clicked, PFindDialog, qOverload<>(&QDialog::accept));
        QObject::connect(fFind_comboBox, SIGNAL(editTextChanged(QString)), PFindDialog, SLOT(onFindTextAvailable(QString)));

        QMetaObject::connectSlotsByName(PFindDialog);
    } // setupUi

    void retranslateUi(QDialog *PFindDialog)
    {
        PFindDialog->setWindowTitle(QCoreApplication::translate("PFindDialog", "Find Text - musredit", nullptr));
        fFind_groupBox->setTitle(QCoreApplication::translate("PFindDialog", "Find", nullptr));
        fFind_label->setText(QCoreApplication::translate("PFindDialog", "Text to find:", nullptr));
        fOptions_groupBox->setTitle(QCoreApplication::translate("PFindDialog", "Options", nullptr));
        fCaseSensitive_checkBox->setText(QCoreApplication::translate("PFindDialog", "C&ase Sensitive", nullptr));
        fFindBackwards_checkBox->setText(QCoreApplication::translate("PFindDialog", "Find &backwards", nullptr));
        fWholeWordsOnly_checkBox->setText(QCoreApplication::translate("PFindDialog", "&Whole words only", nullptr));
        fSelectedText_checkBox->setText(QCoreApplication::translate("PFindDialog", "&Selected text", nullptr));
        fFromCursor_checkBox->setText(QCoreApplication::translate("PFindDialog", "From c&ursor", nullptr));
        fFind_pushButton->setText(QCoreApplication::translate("PFindDialog", "&Find", nullptr));
        fClose_pushButton->setText(QCoreApplication::translate("PFindDialog", "&Close", nullptr));
    } // retranslateUi

};

namespace Ui {
    class PFindDialog: public Ui_PFindDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PFINDDIALOG_H
