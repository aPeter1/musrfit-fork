/********************************************************************************
** Form generated from reading UI file 'PGetPlotBlockDialog.ui'
**
** Created by: Qt User Interface Compiler version 6.4.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PGETPLOTBLOCKDIALOG_H
#define UI_PGETPLOTBLOCKDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PGetPlotBlockDialog
{
public:
    QWidget *widget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_2;
    QGridLayout *gridLayout_2;
    QLabel *fType_label;
    QComboBox *fType_comboBox;
    QLabel *fRunList_label;
    QLineEdit *fRunList_lineEdit;
    QGridLayout *gridLayout;
    QLabel *fXRange_label;
    QLineEdit *fXRangeLow_lineEdit;
    QLineEdit *fXRangeUp_lineEdit;
    QLabel *fYRange_label;
    QLineEdit *fYRangeLow_lineEdit;
    QLineEdit *fYRangeUp_lineEdit;
    QPushButton *fAdd_pushButton;
    QPlainTextEdit *fPlot_plainTextEdit;
    QHBoxLayout *horizontalLayout;
    QPushButton *fHelp_pushButton;
    QSpacerItem *horizontalSpacer;
    QPushButton *fOk_pushButton;
    QPushButton *fCancel_pushButton;

    void setupUi(QDialog *PGetPlotBlockDialog)
    {
        if (PGetPlotBlockDialog->objectName().isEmpty())
            PGetPlotBlockDialog->setObjectName("PGetPlotBlockDialog");
        PGetPlotBlockDialog->setWindowModality(Qt::WindowModal);
        PGetPlotBlockDialog->resize(719, 312);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/musredit/musrfit.xpm"), QSize(), QIcon::Normal, QIcon::Off);
        PGetPlotBlockDialog->setWindowIcon(icon);
        widget = new QWidget(PGetPlotBlockDialog);
        widget->setObjectName("widget");
        widget->setGeometry(QRect(10, 10, 702, 292));
        verticalLayout = new QVBoxLayout(widget);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        gridLayout_2 = new QGridLayout();
        gridLayout_2->setObjectName("gridLayout_2");
        fType_label = new QLabel(widget);
        fType_label->setObjectName("fType_label");
        QFont font;
        font.setPointSize(9);
        font.setBold(true);
        fType_label->setFont(font);

        gridLayout_2->addWidget(fType_label, 0, 0, 1, 1);

        fType_comboBox = new QComboBox(widget);
        fType_comboBox->addItem(QString());
        fType_comboBox->addItem(QString());
        fType_comboBox->addItem(QString());
        fType_comboBox->addItem(QString());
        fType_comboBox->setObjectName("fType_comboBox");

        gridLayout_2->addWidget(fType_comboBox, 0, 1, 1, 1);

        fRunList_label = new QLabel(widget);
        fRunList_label->setObjectName("fRunList_label");
        fRunList_label->setFont(font);

        gridLayout_2->addWidget(fRunList_label, 1, 0, 1, 1);

        fRunList_lineEdit = new QLineEdit(widget);
        fRunList_lineEdit->setObjectName("fRunList_lineEdit");

        gridLayout_2->addWidget(fRunList_lineEdit, 1, 1, 1, 1);


        horizontalLayout_2->addLayout(gridLayout_2);

        gridLayout = new QGridLayout();
        gridLayout->setObjectName("gridLayout");
        fXRange_label = new QLabel(widget);
        fXRange_label->setObjectName("fXRange_label");
        fXRange_label->setFont(font);

        gridLayout->addWidget(fXRange_label, 0, 0, 1, 1);

        fXRangeLow_lineEdit = new QLineEdit(widget);
        fXRangeLow_lineEdit->setObjectName("fXRangeLow_lineEdit");

        gridLayout->addWidget(fXRangeLow_lineEdit, 0, 1, 1, 1);

        fXRangeUp_lineEdit = new QLineEdit(widget);
        fXRangeUp_lineEdit->setObjectName("fXRangeUp_lineEdit");

        gridLayout->addWidget(fXRangeUp_lineEdit, 0, 2, 1, 1);

        fYRange_label = new QLabel(widget);
        fYRange_label->setObjectName("fYRange_label");
        fYRange_label->setFont(font);

        gridLayout->addWidget(fYRange_label, 1, 0, 1, 1);

        fYRangeLow_lineEdit = new QLineEdit(widget);
        fYRangeLow_lineEdit->setObjectName("fYRangeLow_lineEdit");

        gridLayout->addWidget(fYRangeLow_lineEdit, 1, 1, 1, 1);

        fYRangeUp_lineEdit = new QLineEdit(widget);
        fYRangeUp_lineEdit->setObjectName("fYRangeUp_lineEdit");

        gridLayout->addWidget(fYRangeUp_lineEdit, 1, 2, 1, 1);


        horizontalLayout_2->addLayout(gridLayout);

        fAdd_pushButton = new QPushButton(widget);
        fAdd_pushButton->setObjectName("fAdd_pushButton");

        horizontalLayout_2->addWidget(fAdd_pushButton);


        verticalLayout->addLayout(horizontalLayout_2);

        fPlot_plainTextEdit = new QPlainTextEdit(widget);
        fPlot_plainTextEdit->setObjectName("fPlot_plainTextEdit");

        verticalLayout->addWidget(fPlot_plainTextEdit);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        fHelp_pushButton = new QPushButton(widget);
        fHelp_pushButton->setObjectName("fHelp_pushButton");

        horizontalLayout->addWidget(fHelp_pushButton);

        horizontalSpacer = new QSpacerItem(118, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        fOk_pushButton = new QPushButton(widget);
        fOk_pushButton->setObjectName("fOk_pushButton");

        horizontalLayout->addWidget(fOk_pushButton);

        fCancel_pushButton = new QPushButton(widget);
        fCancel_pushButton->setObjectName("fCancel_pushButton");

        horizontalLayout->addWidget(fCancel_pushButton);


        verticalLayout->addLayout(horizontalLayout);

        QWidget::setTabOrder(fOk_pushButton, fCancel_pushButton);
        QWidget::setTabOrder(fCancel_pushButton, fHelp_pushButton);

        retranslateUi(PGetPlotBlockDialog);
        QObject::connect(fCancel_pushButton, &QPushButton::clicked, PGetPlotBlockDialog, qOverload<>(&QDialog::reject));
        QObject::connect(fOk_pushButton, &QPushButton::clicked, PGetPlotBlockDialog, qOverload<>(&QDialog::accept));
        QObject::connect(fHelp_pushButton, SIGNAL(clicked()), PGetPlotBlockDialog, SLOT(helpContent()));
        QObject::connect(fAdd_pushButton, SIGNAL(clicked()), PGetPlotBlockDialog, SLOT(addPlot()));

        QMetaObject::connectSlotsByName(PGetPlotBlockDialog);
    } // setupUi

    void retranslateUi(QDialog *PGetPlotBlockDialog)
    {
        PGetPlotBlockDialog->setWindowTitle(QCoreApplication::translate("PGetPlotBlockDialog", "Get Plot", nullptr));
        fType_label->setText(QCoreApplication::translate("PGetPlotBlockDialog", "Type", nullptr));
        fType_comboBox->setItemText(0, QCoreApplication::translate("PGetPlotBlockDialog", "Single Histo", nullptr));
        fType_comboBox->setItemText(1, QCoreApplication::translate("PGetPlotBlockDialog", "Asymmetry", nullptr));
        fType_comboBox->setItemText(2, QCoreApplication::translate("PGetPlotBlockDialog", "MuMinus", nullptr));
        fType_comboBox->setItemText(3, QCoreApplication::translate("PGetPlotBlockDialog", "NonMusr", nullptr));

        fRunList_label->setText(QCoreApplication::translate("PGetPlotBlockDialog", "Run List", nullptr));
        fXRange_label->setText(QCoreApplication::translate("PGetPlotBlockDialog", "Time-/x-Range", nullptr));
        fYRange_label->setText(QCoreApplication::translate("PGetPlotBlockDialog", "y-Range", nullptr));
        fAdd_pushButton->setText(QCoreApplication::translate("PGetPlotBlockDialog", "&Add", nullptr));
        fHelp_pushButton->setText(QCoreApplication::translate("PGetPlotBlockDialog", "&Help", nullptr));
        fOk_pushButton->setText(QCoreApplication::translate("PGetPlotBlockDialog", "&OK", nullptr));
        fCancel_pushButton->setText(QCoreApplication::translate("PGetPlotBlockDialog", "&Cancel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class PGetPlotBlockDialog: public Ui_PGetPlotBlockDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PGETPLOTBLOCKDIALOG_H
