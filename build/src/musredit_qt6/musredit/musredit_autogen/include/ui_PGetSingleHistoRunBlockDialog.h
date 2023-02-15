/********************************************************************************
** Form generated from reading UI file 'PGetSingleHistoRunBlockDialog.ui'
**
** Created by: Qt User Interface Compiler version 6.4.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PGETSINGLEHISTORUNBLOCKDIALOG_H
#define UI_PGETSINGLEHISTORUNBLOCKDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PGetSingleHistoRunBlockDialog
{
public:
    QGroupBox *fRunHeaderInfo_groupBox;
    QLabel *fRunFileName_label;
    QLabel *fBeamline_label;
    QLabel *fInstitute_label;
    QLabel *fFileFormat_label;
    QLineEdit *fRunFileName_lineEdit;
    QLineEdit *fBeamline_lineEdit;
    QComboBox *fInstitute_comboBox;
    QComboBox *fFileFormat_comboBox;
    QGroupBox *fRequiredEntries_groupBox;
    QLabel *fMap_label;
    QLabel *fForwardHistoNo_label;
    QLabel *fDataRange_label;
    QLabel *fBackgroundFix_label;
    QLabel *fOr_label;
    QLabel *fBackgroundRange_label;
    QLabel *fFitRange_label;
    QLineEdit *fMap_lineEdit;
    QLineEdit *fForwardHistoNo_lineEdit;
    QLineEdit *fDataStart_lineEdit;
    QLineEdit *fDataEnd_lineEdit;
    QLineEdit *fBackgroundFix_lineEdit;
    QLineEdit *fBackgroundStart_lineEdit;
    QLineEdit *fBackgroundEnd_lineEdit;
    QLineEdit *fFitRangeStart_lineEdit;
    QLineEdit *fFitRangeEnd_lineEdit;
    QLineEdit *fPacking_lineEdit;
    QLabel *fPacking_textLabel;
    QLabel *fNorm_label;
    QLineEdit *fNorm_lineEdit;
    QLabel *fBackgroundFit_label;
    QLineEdit *fBackgroundFit_lineEdit;
    QGroupBox *fOptionalEntries_groupBox;
    QLabel *fT0_label;
    QLineEdit *fT0_lineEdit;
    QLabel *fMuonLifetimeParameter_label;
    QLineEdit *fLifetime_lineEdit;
    QCheckBox *fLifetimeCorrection_checkBox;
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout;
    QFrame *line;
    QHBoxLayout *horizontalLayout;
    QPushButton *fHelp_pushButton;
    QSpacerItem *horizontalSpacer;
    QPushButton *fOk_pushButton;
    QPushButton *fCancel_pushButton;

    void setupUi(QDialog *PGetSingleHistoRunBlockDialog)
    {
        if (PGetSingleHistoRunBlockDialog->objectName().isEmpty())
            PGetSingleHistoRunBlockDialog->setObjectName("PGetSingleHistoRunBlockDialog");
        PGetSingleHistoRunBlockDialog->setWindowModality(Qt::WindowModal);
        PGetSingleHistoRunBlockDialog->resize(440, 592);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/images/musrfit.xpm"), QSize(), QIcon::Normal, QIcon::Off);
        PGetSingleHistoRunBlockDialog->setWindowIcon(icon);
        fRunHeaderInfo_groupBox = new QGroupBox(PGetSingleHistoRunBlockDialog);
        fRunHeaderInfo_groupBox->setObjectName("fRunHeaderInfo_groupBox");
        fRunHeaderInfo_groupBox->setGeometry(QRect(10, 0, 421, 121));
        fRunFileName_label = new QLabel(fRunHeaderInfo_groupBox);
        fRunFileName_label->setObjectName("fRunFileName_label");
        fRunFileName_label->setGeometry(QRect(20, 30, 91, 16));
        fBeamline_label = new QLabel(fRunHeaderInfo_groupBox);
        fBeamline_label->setObjectName("fBeamline_label");
        fBeamline_label->setGeometry(QRect(20, 60, 71, 16));
        fInstitute_label = new QLabel(fRunHeaderInfo_groupBox);
        fInstitute_label->setObjectName("fInstitute_label");
        fInstitute_label->setGeometry(QRect(20, 90, 56, 16));
        fFileFormat_label = new QLabel(fRunHeaderInfo_groupBox);
        fFileFormat_label->setObjectName("fFileFormat_label");
        fFileFormat_label->setGeometry(QRect(240, 90, 91, 16));
        fRunFileName_lineEdit = new QLineEdit(fRunHeaderInfo_groupBox);
        fRunFileName_lineEdit->setObjectName("fRunFileName_lineEdit");
        fRunFileName_lineEdit->setGeometry(QRect(120, 26, 291, 26));
        fBeamline_lineEdit = new QLineEdit(fRunHeaderInfo_groupBox);
        fBeamline_lineEdit->setObjectName("fBeamline_lineEdit");
        fBeamline_lineEdit->setGeometry(QRect(120, 56, 113, 26));
        fInstitute_comboBox = new QComboBox(fRunHeaderInfo_groupBox);
        fInstitute_comboBox->addItem(QString());
        fInstitute_comboBox->addItem(QString());
        fInstitute_comboBox->addItem(QString());
        fInstitute_comboBox->addItem(QString());
        fInstitute_comboBox->setObjectName("fInstitute_comboBox");
        fInstitute_comboBox->setGeometry(QRect(120, 86, 77, 25));
        fFileFormat_comboBox = new QComboBox(fRunHeaderInfo_groupBox);
        fFileFormat_comboBox->addItem(QString());
        fFileFormat_comboBox->addItem(QString());
        fFileFormat_comboBox->addItem(QString());
        fFileFormat_comboBox->addItem(QString());
        fFileFormat_comboBox->addItem(QString());
        fFileFormat_comboBox->addItem(QString());
        fFileFormat_comboBox->addItem(QString());
        fFileFormat_comboBox->addItem(QString());
        fFileFormat_comboBox->addItem(QString());
        fFileFormat_comboBox->setObjectName("fFileFormat_comboBox");
        fFileFormat_comboBox->setGeometry(QRect(316, 86, 91, 25));
        fRequiredEntries_groupBox = new QGroupBox(PGetSingleHistoRunBlockDialog);
        fRequiredEntries_groupBox->setObjectName("fRequiredEntries_groupBox");
        fRequiredEntries_groupBox->setGeometry(QRect(10, 120, 421, 301));
        fMap_label = new QLabel(fRequiredEntries_groupBox);
        fMap_label->setObjectName("fMap_label");
        fMap_label->setGeometry(QRect(20, 30, 56, 16));
        fForwardHistoNo_label = new QLabel(fRequiredEntries_groupBox);
        fForwardHistoNo_label->setObjectName("fForwardHistoNo_label");
        fForwardHistoNo_label->setGeometry(QRect(20, 60, 111, 20));
        fDataRange_label = new QLabel(fRequiredEntries_groupBox);
        fDataRange_label->setObjectName("fDataRange_label");
        fDataRange_label->setGeometry(QRect(20, 120, 81, 16));
        fBackgroundFix_label = new QLabel(fRequiredEntries_groupBox);
        fBackgroundFix_label->setObjectName("fBackgroundFix_label");
        fBackgroundFix_label->setGeometry(QRect(20, 150, 111, 16));
        fOr_label = new QLabel(fRequiredEntries_groupBox);
        fOr_label->setObjectName("fOr_label");
        fOr_label->setGeometry(QRect(240, 150, 31, 16));
        QFont font;
        font.setBold(true);
        fOr_label->setFont(font);
        fBackgroundRange_label = new QLabel(fRequiredEntries_groupBox);
        fBackgroundRange_label->setObjectName("fBackgroundRange_label");
        fBackgroundRange_label->setGeometry(QRect(20, 210, 121, 16));
        fFitRange_label = new QLabel(fRequiredEntries_groupBox);
        fFitRange_label->setObjectName("fFitRange_label");
        fFitRange_label->setGeometry(QRect(20, 240, 71, 16));
        fMap_lineEdit = new QLineEdit(fRequiredEntries_groupBox);
        fMap_lineEdit->setObjectName("fMap_lineEdit");
        fMap_lineEdit->setGeometry(QRect(60, 26, 351, 26));
        fForwardHistoNo_lineEdit = new QLineEdit(fRequiredEntries_groupBox);
        fForwardHistoNo_lineEdit->setObjectName("fForwardHistoNo_lineEdit");
        fForwardHistoNo_lineEdit->setGeometry(QRect(150, 56, 71, 26));
        fDataStart_lineEdit = new QLineEdit(fRequiredEntries_groupBox);
        fDataStart_lineEdit->setObjectName("fDataStart_lineEdit");
        fDataStart_lineEdit->setGeometry(QRect(150, 116, 71, 26));
        fDataEnd_lineEdit = new QLineEdit(fRequiredEntries_groupBox);
        fDataEnd_lineEdit->setObjectName("fDataEnd_lineEdit");
        fDataEnd_lineEdit->setGeometry(QRect(220, 116, 71, 26));
        fBackgroundFix_lineEdit = new QLineEdit(fRequiredEntries_groupBox);
        fBackgroundFix_lineEdit->setObjectName("fBackgroundFix_lineEdit");
        fBackgroundFix_lineEdit->setGeometry(QRect(150, 146, 71, 26));
        fBackgroundStart_lineEdit = new QLineEdit(fRequiredEntries_groupBox);
        fBackgroundStart_lineEdit->setObjectName("fBackgroundStart_lineEdit");
        fBackgroundStart_lineEdit->setGeometry(QRect(150, 206, 71, 26));
        fBackgroundEnd_lineEdit = new QLineEdit(fRequiredEntries_groupBox);
        fBackgroundEnd_lineEdit->setObjectName("fBackgroundEnd_lineEdit");
        fBackgroundEnd_lineEdit->setGeometry(QRect(220, 206, 71, 26));
        fFitRangeStart_lineEdit = new QLineEdit(fRequiredEntries_groupBox);
        fFitRangeStart_lineEdit->setObjectName("fFitRangeStart_lineEdit");
        fFitRangeStart_lineEdit->setGeometry(QRect(150, 236, 71, 26));
        fFitRangeEnd_lineEdit = new QLineEdit(fRequiredEntries_groupBox);
        fFitRangeEnd_lineEdit->setObjectName("fFitRangeEnd_lineEdit");
        fFitRangeEnd_lineEdit->setGeometry(QRect(220, 236, 71, 26));
        fPacking_lineEdit = new QLineEdit(fRequiredEntries_groupBox);
        fPacking_lineEdit->setObjectName("fPacking_lineEdit");
        fPacking_lineEdit->setGeometry(QRect(150, 266, 71, 26));
        fPacking_textLabel = new QLabel(fRequiredEntries_groupBox);
        fPacking_textLabel->setObjectName("fPacking_textLabel");
        fPacking_textLabel->setGeometry(QRect(20, 270, 111, 16));
        fNorm_label = new QLabel(fRequiredEntries_groupBox);
        fNorm_label->setObjectName("fNorm_label");
        fNorm_label->setGeometry(QRect(20, 90, 56, 20));
        fNorm_lineEdit = new QLineEdit(fRequiredEntries_groupBox);
        fNorm_lineEdit->setObjectName("fNorm_lineEdit");
        fNorm_lineEdit->setGeometry(QRect(150, 86, 71, 26));
        fBackgroundFit_label = new QLabel(fRequiredEntries_groupBox);
        fBackgroundFit_label->setObjectName("fBackgroundFit_label");
        fBackgroundFit_label->setGeometry(QRect(20, 180, 101, 16));
        fBackgroundFit_lineEdit = new QLineEdit(fRequiredEntries_groupBox);
        fBackgroundFit_lineEdit->setObjectName("fBackgroundFit_lineEdit");
        fBackgroundFit_lineEdit->setGeometry(QRect(150, 176, 71, 26));
        fOptionalEntries_groupBox = new QGroupBox(PGetSingleHistoRunBlockDialog);
        fOptionalEntries_groupBox->setObjectName("fOptionalEntries_groupBox");
        fOptionalEntries_groupBox->setGeometry(QRect(10, 420, 421, 121));
        fT0_label = new QLabel(fOptionalEntries_groupBox);
        fT0_label->setObjectName("fT0_label");
        fT0_label->setGeometry(QRect(20, 30, 241, 16));
        fT0_lineEdit = new QLineEdit(fOptionalEntries_groupBox);
        fT0_lineEdit->setObjectName("fT0_lineEdit");
        fT0_lineEdit->setGeometry(QRect(260, 26, 71, 26));
        fMuonLifetimeParameter_label = new QLabel(fOptionalEntries_groupBox);
        fMuonLifetimeParameter_label->setObjectName("fMuonLifetimeParameter_label");
        fMuonLifetimeParameter_label->setGeometry(QRect(20, 60, 211, 16));
        fLifetime_lineEdit = new QLineEdit(fOptionalEntries_groupBox);
        fLifetime_lineEdit->setObjectName("fLifetime_lineEdit");
        fLifetime_lineEdit->setGeometry(QRect(180, 56, 71, 26));
        fLifetimeCorrection_checkBox = new QCheckBox(fOptionalEntries_groupBox);
        fLifetimeCorrection_checkBox->setObjectName("fLifetimeCorrection_checkBox");
        fLifetimeCorrection_checkBox->setGeometry(QRect(18, 90, 151, 23));
        layoutWidget = new QWidget(PGetSingleHistoRunBlockDialog);
        layoutWidget->setObjectName("layoutWidget");
        layoutWidget->setGeometry(QRect(10, 545, 421, 39));
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

        horizontalSpacer = new QSpacerItem(188, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        fOk_pushButton = new QPushButton(layoutWidget);
        fOk_pushButton->setObjectName("fOk_pushButton");

        horizontalLayout->addWidget(fOk_pushButton);

        fCancel_pushButton = new QPushButton(layoutWidget);
        fCancel_pushButton->setObjectName("fCancel_pushButton");

        horizontalLayout->addWidget(fCancel_pushButton);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(PGetSingleHistoRunBlockDialog);
        QObject::connect(fCancel_pushButton, &QPushButton::clicked, PGetSingleHistoRunBlockDialog, qOverload<>(&QDialog::reject));
        QObject::connect(fOk_pushButton, &QPushButton::clicked, PGetSingleHistoRunBlockDialog, qOverload<>(&QDialog::accept));
        QObject::connect(fHelp_pushButton, SIGNAL(clicked()), PGetSingleHistoRunBlockDialog, SLOT(helpContent()));

        QMetaObject::connectSlotsByName(PGetSingleHistoRunBlockDialog);
    } // setupUi

    void retranslateUi(QDialog *PGetSingleHistoRunBlockDialog)
    {
        PGetSingleHistoRunBlockDialog->setWindowTitle(QCoreApplication::translate("PGetSingleHistoRunBlockDialog", "Get Single Histogram Run Block Parameters", nullptr));
        fRunHeaderInfo_groupBox->setTitle(QCoreApplication::translate("PGetSingleHistoRunBlockDialog", "Run Header Info", nullptr));
        fRunFileName_label->setText(QCoreApplication::translate("PGetSingleHistoRunBlockDialog", "Run File Name", nullptr));
        fBeamline_label->setText(QCoreApplication::translate("PGetSingleHistoRunBlockDialog", "Beamline", nullptr));
        fInstitute_label->setText(QCoreApplication::translate("PGetSingleHistoRunBlockDialog", "Institute", nullptr));
        fFileFormat_label->setText(QCoreApplication::translate("PGetSingleHistoRunBlockDialog", "File Format", nullptr));
        fInstitute_comboBox->setItemText(0, QCoreApplication::translate("PGetSingleHistoRunBlockDialog", "PSI", nullptr));
        fInstitute_comboBox->setItemText(1, QCoreApplication::translate("PGetSingleHistoRunBlockDialog", "RAL", nullptr));
        fInstitute_comboBox->setItemText(2, QCoreApplication::translate("PGetSingleHistoRunBlockDialog", "TRIUMF", nullptr));
        fInstitute_comboBox->setItemText(3, QCoreApplication::translate("PGetSingleHistoRunBlockDialog", "JPARC", nullptr));

        fFileFormat_comboBox->setItemText(0, QCoreApplication::translate("PGetSingleHistoRunBlockDialog", "NeXuS", nullptr));
        fFileFormat_comboBox->setItemText(1, QCoreApplication::translate("PGetSingleHistoRunBlockDialog", "ROOT-NPP", nullptr));
        fFileFormat_comboBox->setItemText(2, QCoreApplication::translate("PGetSingleHistoRunBlockDialog", "ROOT-PPC", nullptr));
        fFileFormat_comboBox->setItemText(3, QCoreApplication::translate("PGetSingleHistoRunBlockDialog", "PSI-BIN", nullptr));
        fFileFormat_comboBox->setItemText(4, QCoreApplication::translate("PGetSingleHistoRunBlockDialog", "PSI-MDU", nullptr));
        fFileFormat_comboBox->setItemText(5, QCoreApplication::translate("PGetSingleHistoRunBlockDialog", "MUD", nullptr));
        fFileFormat_comboBox->setItemText(6, QCoreApplication::translate("PGetSingleHistoRunBlockDialog", "MDU-ASCII", nullptr));
        fFileFormat_comboBox->setItemText(7, QCoreApplication::translate("PGetSingleHistoRunBlockDialog", "ASCII", nullptr));
        fFileFormat_comboBox->setItemText(8, QCoreApplication::translate("PGetSingleHistoRunBlockDialog", "DB", nullptr));

        fRequiredEntries_groupBox->setTitle(QCoreApplication::translate("PGetSingleHistoRunBlockDialog", "Required Entries", nullptr));
        fMap_label->setText(QCoreApplication::translate("PGetSingleHistoRunBlockDialog", "Map", nullptr));
        fForwardHistoNo_label->setText(QCoreApplication::translate("PGetSingleHistoRunBlockDialog", "Forward Histo No", nullptr));
        fDataRange_label->setText(QCoreApplication::translate("PGetSingleHistoRunBlockDialog", "Data  Range", nullptr));
        fBackgroundFix_label->setText(QCoreApplication::translate("PGetSingleHistoRunBlockDialog", "Background Fix", nullptr));
        fOr_label->setText(QCoreApplication::translate("PGetSingleHistoRunBlockDialog", "OR", nullptr));
        fBackgroundRange_label->setText(QCoreApplication::translate("PGetSingleHistoRunBlockDialog", "Background Range", nullptr));
        fFitRange_label->setText(QCoreApplication::translate("PGetSingleHistoRunBlockDialog", "Fit Range", nullptr));
        fMap_lineEdit->setText(QCoreApplication::translate("PGetSingleHistoRunBlockDialog", "0  0  0  0  0  0  0  0  0", nullptr));
        fPacking_lineEdit->setText(QCoreApplication::translate("PGetSingleHistoRunBlockDialog", "1", nullptr));
        fPacking_textLabel->setText(QCoreApplication::translate("PGetSingleHistoRunBlockDialog", "Packing/Binning", nullptr));
        fNorm_label->setText(QCoreApplication::translate("PGetSingleHistoRunBlockDialog", "Norm", nullptr));
        fBackgroundFit_label->setText(QCoreApplication::translate("PGetSingleHistoRunBlockDialog", "Background Fit", nullptr));
        fOptionalEntries_groupBox->setTitle(QCoreApplication::translate("PGetSingleHistoRunBlockDialog", "Optional Entries", nullptr));
        fT0_label->setText(QCoreApplication::translate("PGetSingleHistoRunBlockDialog", "t0 (reqired if not given in the data file)", nullptr));
        fMuonLifetimeParameter_label->setText(QCoreApplication::translate("PGetSingleHistoRunBlockDialog", "Muon Lifetime Parameter", nullptr));
        fLifetimeCorrection_checkBox->setText(QCoreApplication::translate("PGetSingleHistoRunBlockDialog", "Lifetime Correction", nullptr));
        fHelp_pushButton->setText(QCoreApplication::translate("PGetSingleHistoRunBlockDialog", "&Help", nullptr));
        fOk_pushButton->setText(QCoreApplication::translate("PGetSingleHistoRunBlockDialog", "&OK", nullptr));
        fCancel_pushButton->setText(QCoreApplication::translate("PGetSingleHistoRunBlockDialog", "&Cancel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class PGetSingleHistoRunBlockDialog: public Ui_PGetSingleHistoRunBlockDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PGETSINGLEHISTORUNBLOCKDIALOG_H
