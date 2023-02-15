/********************************************************************************
** Form generated from reading UI file 'PPrefsDialog.ui'
**
** Created by: Qt User Interface Compiler version 6.4.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PPREFSDIALOG_H
#define UI_PPREFSDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PPrefsDialog
{
public:
    QWidget *layoutWidget_0;
    QVBoxLayout *verticalLayout;
    QTabWidget *fTabWidget;
    QWidget *fGeneral_tab;
    QPushButton *fDefaultPath_pushButton;
    QWidget *layoutWidget;
    QHBoxLayout *horizontalLayout_2;
    QLineEdit *fTimeout_lineEdit;
    QLabel *fTimeout_label;
    QSpacerItem *horizontalSpacer_2;
    QVBoxLayout *verticalLayout_2;
    QCheckBox *fDarkThemeIconsMenu_checkBox;
    QCheckBox *fDarkThemeIconsToolbar_checkBox;
    QWidget *fMusrfit_tab;
    QCheckBox *fKeepMn2Output_checkBox;
    QCheckBox *fDumpAscii_checkBox;
    QCheckBox *fDumpRoot_checkBox;
    QCheckBox *fTitleFromData_checkBox;
    QCheckBox *fPerRunBlockChisq_checkBox;
    QCheckBox *fEstimateN0_checkBox;
    QWidget *fMusrview_tab;
    QCheckBox *fFourier_checkBox;
    QCheckBox *fAvg_checkBox;
    QCheckBox *fOneToOne_checkBox;
    QWidget *fMusrt0_tab;
    QCheckBox *fEnableMusrT0_checkBox;
    QHBoxLayout *horizontalLayout;
    QPushButton *fHelp_pushButton;
    QSpacerItem *horizontalSpacer;
    QPushButton *fOk_pushButton;
    QPushButton *fCancel_pushButton;

    void setupUi(QDialog *PPrefsDialog)
    {
        if (PPrefsDialog->objectName().isEmpty())
            PPrefsDialog->setObjectName("PPrefsDialog");
        PPrefsDialog->setWindowModality(Qt::WindowModal);
        PPrefsDialog->resize(452, 180);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/images/musrfit.xpm"), QSize(), QIcon::Normal, QIcon::Off);
        PPrefsDialog->setWindowIcon(icon);
        layoutWidget_0 = new QWidget(PPrefsDialog);
        layoutWidget_0->setObjectName("layoutWidget_0");
        layoutWidget_0->setGeometry(QRect(0, 5, 451, 171));
        verticalLayout = new QVBoxLayout(layoutWidget_0);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        fTabWidget = new QTabWidget(layoutWidget_0);
        fTabWidget->setObjectName("fTabWidget");
        fGeneral_tab = new QWidget();
        fGeneral_tab->setObjectName("fGeneral_tab");
        fDefaultPath_pushButton = new QPushButton(fGeneral_tab);
        fDefaultPath_pushButton->setObjectName("fDefaultPath_pushButton");
        fDefaultPath_pushButton->setGeometry(QRect(10, 70, 421, 31));
        layoutWidget = new QWidget(fGeneral_tab);
        layoutWidget->setObjectName("layoutWidget");
        layoutWidget->setGeometry(QRect(12, 10, 415, 62));
        horizontalLayout_2 = new QHBoxLayout(layoutWidget);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        fTimeout_lineEdit = new QLineEdit(layoutWidget);
        fTimeout_lineEdit->setObjectName("fTimeout_lineEdit");

        horizontalLayout_2->addWidget(fTimeout_lineEdit);

        fTimeout_label = new QLabel(layoutWidget);
        fTimeout_label->setObjectName("fTimeout_label");

        horizontalLayout_2->addWidget(fTimeout_label);

        horizontalSpacer_2 = new QSpacerItem(38, 30, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName("verticalLayout_2");
        fDarkThemeIconsMenu_checkBox = new QCheckBox(layoutWidget);
        fDarkThemeIconsMenu_checkBox->setObjectName("fDarkThemeIconsMenu_checkBox");

        verticalLayout_2->addWidget(fDarkThemeIconsMenu_checkBox);

        fDarkThemeIconsToolbar_checkBox = new QCheckBox(layoutWidget);
        fDarkThemeIconsToolbar_checkBox->setObjectName("fDarkThemeIconsToolbar_checkBox");

        verticalLayout_2->addWidget(fDarkThemeIconsToolbar_checkBox);


        horizontalLayout_2->addLayout(verticalLayout_2);

        fTabWidget->addTab(fGeneral_tab, QString());
        fMusrfit_tab = new QWidget();
        fMusrfit_tab->setObjectName("fMusrfit_tab");
        fKeepMn2Output_checkBox = new QCheckBox(fMusrfit_tab);
        fKeepMn2Output_checkBox->setObjectName("fKeepMn2Output_checkBox");
        fKeepMn2Output_checkBox->setGeometry(QRect(10, 10, 161, 23));
        fDumpAscii_checkBox = new QCheckBox(fMusrfit_tab);
        fDumpAscii_checkBox->setObjectName("fDumpAscii_checkBox");
        fDumpAscii_checkBox->setGeometry(QRect(10, 35, 151, 23));
        fDumpRoot_checkBox = new QCheckBox(fMusrfit_tab);
        fDumpRoot_checkBox->setObjectName("fDumpRoot_checkBox");
        fDumpRoot_checkBox->setGeometry(QRect(10, 60, 131, 23));
        fTitleFromData_checkBox = new QCheckBox(fMusrfit_tab);
        fTitleFromData_checkBox->setObjectName("fTitleFromData_checkBox");
        fTitleFromData_checkBox->setGeometry(QRect(190, 10, 241, 23));
        fPerRunBlockChisq_checkBox = new QCheckBox(fMusrfit_tab);
        fPerRunBlockChisq_checkBox->setObjectName("fPerRunBlockChisq_checkBox");
        fPerRunBlockChisq_checkBox->setGeometry(QRect(190, 35, 241, 22));
        fEstimateN0_checkBox = new QCheckBox(fMusrfit_tab);
        fEstimateN0_checkBox->setObjectName("fEstimateN0_checkBox");
        fEstimateN0_checkBox->setGeometry(QRect(190, 60, 241, 22));
        fTabWidget->addTab(fMusrfit_tab, QString());
        fMusrview_tab = new QWidget();
        fMusrview_tab->setObjectName("fMusrview_tab");
        fFourier_checkBox = new QCheckBox(fMusrview_tab);
        fFourier_checkBox->setObjectName("fFourier_checkBox");
        fFourier_checkBox->setGeometry(QRect(10, 10, 421, 22));
        fAvg_checkBox = new QCheckBox(fMusrview_tab);
        fAvg_checkBox->setObjectName("fAvg_checkBox");
        fAvg_checkBox->setGeometry(QRect(10, 40, 421, 21));
        fOneToOne_checkBox = new QCheckBox(fMusrview_tab);
        fOneToOne_checkBox->setObjectName("fOneToOne_checkBox");
        fOneToOne_checkBox->setGeometry(QRect(260, 10, 181, 26));
        fTabWidget->addTab(fMusrview_tab, QString());
        fMusrt0_tab = new QWidget();
        fMusrt0_tab->setObjectName("fMusrt0_tab");
        fEnableMusrT0_checkBox = new QCheckBox(fMusrt0_tab);
        fEnableMusrT0_checkBox->setObjectName("fEnableMusrT0_checkBox");
        fEnableMusrT0_checkBox->setGeometry(QRect(10, 10, 131, 23));
        fTabWidget->addTab(fMusrt0_tab, QString());

        verticalLayout->addWidget(fTabWidget);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        fHelp_pushButton = new QPushButton(layoutWidget_0);
        fHelp_pushButton->setObjectName("fHelp_pushButton");

        horizontalLayout->addWidget(fHelp_pushButton);

        horizontalSpacer = new QSpacerItem(88, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        fOk_pushButton = new QPushButton(layoutWidget_0);
        fOk_pushButton->setObjectName("fOk_pushButton");

        horizontalLayout->addWidget(fOk_pushButton);

        fCancel_pushButton = new QPushButton(layoutWidget_0);
        fCancel_pushButton->setObjectName("fCancel_pushButton");

        horizontalLayout->addWidget(fCancel_pushButton);


        verticalLayout->addLayout(horizontalLayout);

        QWidget::setTabOrder(fTabWidget, fKeepMn2Output_checkBox);
        QWidget::setTabOrder(fKeepMn2Output_checkBox, fDumpAscii_checkBox);
        QWidget::setTabOrder(fDumpAscii_checkBox, fDumpRoot_checkBox);
        QWidget::setTabOrder(fDumpRoot_checkBox, fTitleFromData_checkBox);
        QWidget::setTabOrder(fTitleFromData_checkBox, fOk_pushButton);
        QWidget::setTabOrder(fOk_pushButton, fCancel_pushButton);
        QWidget::setTabOrder(fCancel_pushButton, fHelp_pushButton);

        retranslateUi(PPrefsDialog);
        QObject::connect(fCancel_pushButton, &QPushButton::clicked, PPrefsDialog, qOverload<>(&QDialog::reject));
        QObject::connect(fOk_pushButton, &QPushButton::clicked, PPrefsDialog, qOverload<>(&QDialog::accept));

        fTabWidget->setCurrentIndex(2);


        QMetaObject::connectSlotsByName(PPrefsDialog);
    } // setupUi

    void retranslateUi(QDialog *PPrefsDialog)
    {
        PPrefsDialog->setWindowTitle(QCoreApplication::translate("PPrefsDialog", "Preferences", nullptr));
        fDefaultPath_pushButton->setText(QCoreApplication::translate("PPrefsDialog", "Change Default Search Paths", nullptr));
        fTimeout_label->setText(QCoreApplication::translate("PPrefsDialog", "timeout", nullptr));
        fDarkThemeIconsMenu_checkBox->setText(QCoreApplication::translate("PPrefsDialog", "Dark Theme Icons Menu", nullptr));
        fDarkThemeIconsToolbar_checkBox->setText(QCoreApplication::translate("PPrefsDialog", "Dark Theme Icons Toolbar", nullptr));
        fTabWidget->setTabText(fTabWidget->indexOf(fGeneral_tab), QCoreApplication::translate("PPrefsDialog", "general", nullptr));
        fKeepMn2Output_checkBox->setText(QCoreApplication::translate("PPrefsDialog", "keep minuit2 output", nullptr));
        fDumpAscii_checkBox->setText(QCoreApplication::translate("PPrefsDialog", "dump ascii", nullptr));
        fDumpRoot_checkBox->setText(QCoreApplication::translate("PPrefsDialog", "dump root", nullptr));
        fTitleFromData_checkBox->setText(QCoreApplication::translate("PPrefsDialog", "take title from data file", nullptr));
        fPerRunBlockChisq_checkBox->setText(QCoreApplication::translate("PPrefsDialog", "keep per run block chisq", nullptr));
        fEstimateN0_checkBox->setText(QCoreApplication::translate("PPrefsDialog", "estimate N0", nullptr));
        fTabWidget->setTabText(fTabWidget->indexOf(fMusrfit_tab), QCoreApplication::translate("PPrefsDialog", "musrfit", nullptr));
        fFourier_checkBox->setText(QCoreApplication::translate("PPrefsDialog", "start with Fourier", nullptr));
        fAvg_checkBox->setText(QCoreApplication::translate("PPrefsDialog", "start with averaged data/Fourier", nullptr));
        fOneToOne_checkBox->setText(QCoreApplication::translate("PPrefsDialog", "theo at data points only", nullptr));
        fTabWidget->setTabText(fTabWidget->indexOf(fMusrview_tab), QCoreApplication::translate("PPrefsDialog", "musrview", nullptr));
        fEnableMusrT0_checkBox->setText(QCoreApplication::translate("PPrefsDialog", "enable it", nullptr));
        fTabWidget->setTabText(fTabWidget->indexOf(fMusrt0_tab), QCoreApplication::translate("PPrefsDialog", "musrt0", nullptr));
        fHelp_pushButton->setText(QCoreApplication::translate("PPrefsDialog", "&Help", nullptr));
        fOk_pushButton->setText(QCoreApplication::translate("PPrefsDialog", "&OK", nullptr));
        fCancel_pushButton->setText(QCoreApplication::translate("PPrefsDialog", "&Cancel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class PPrefsDialog: public Ui_PPrefsDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PPREFSDIALOG_H
