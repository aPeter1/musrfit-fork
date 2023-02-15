/********************************************************************************
** Form generated from reading UI file 'PGetAsymmetryRunBlockDialog.ui'
**
** Created by: Qt User Interface Compiler version 6.4.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PGETASYMMETRYRUNBLOCKDIALOG_H
#define UI_PGETASYMMETRYRUNBLOCKDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
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

class Ui_PGetAsymmetryRunBlockDialog
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
    QLabel *fBackwardHistoNo_label;
    QLabel *fDataRange_label;
    QLabel *fBackgroundFix_label;
    QLabel *fOr_label;
    QLabel *fBackgroundRange_label;
    QLabel *fFitRange_label;
    QLineEdit *fMap_lineEdit;
    QLineEdit *fForwardHistoNo_lineEdit;
    QLineEdit *fBackwardHistoNo_lineEdit;
    QLineEdit *fDataForwardStart_lineEdit;
    QLineEdit *fDataForwardEnd_lineEdit;
    QLineEdit *fDataBackwardStart_lineEdit;
    QLineEdit *fDataBackwardEnd_lineEdit;
    QLineEdit *fBackgroundForwardFix_lineEdit;
    QLineEdit *fBackgroundBackwardFix_lineEdit;
    QLineEdit *fBackgroundForwardStart_lineEdit;
    QLineEdit *fBackgroundForwardEnd_lineEdit;
    QLineEdit *fBackgroundBackwardStart_lineEdit;
    QLineEdit *fBackgroundBackwardEnd_lineEdit;
    QLineEdit *fFitRangeStart_lineEdit;
    QLineEdit *fFitRangeEnd_lineEdit;
    QLineEdit *fPacking_lineEdit;
    QLabel *fPacking_textLabel;
    QGroupBox *fOptionalEntries_groupBox;
    QLabel *fAplha_textLabel;
    QLabel *fBeta_label;
    QLabel *fT0_label;
    QLineEdit *fAlpha_lineEdit;
    QLineEdit *fBeta_lineEdit;
    QLineEdit *fT0Forward_lineEdit;
    QLineEdit *fT0Backward_lineEdit;
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout;
    QFrame *line;
    QHBoxLayout *horizontalLayout;
    QPushButton *fHelp_pushButton;
    QSpacerItem *horizontalSpacer;
    QPushButton *fOk_pushButton;
    QPushButton *fCancel_pushButton;

    void setupUi(QDialog *PGetAsymmetryRunBlockDialog)
    {
        if (PGetAsymmetryRunBlockDialog->objectName().isEmpty())
            PGetAsymmetryRunBlockDialog->setObjectName("PGetAsymmetryRunBlockDialog");
        PGetAsymmetryRunBlockDialog->setWindowModality(Qt::WindowModal);
        PGetAsymmetryRunBlockDialog->resize(462, 506);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/images/musrfit.xpm"), QSize(), QIcon::Normal, QIcon::Off);
        PGetAsymmetryRunBlockDialog->setWindowIcon(icon);
        fRunHeaderInfo_groupBox = new QGroupBox(PGetAsymmetryRunBlockDialog);
        fRunHeaderInfo_groupBox->setObjectName("fRunHeaderInfo_groupBox");
        fRunHeaderInfo_groupBox->setGeometry(QRect(10, 0, 441, 121));
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
        fFileFormat_label->setGeometry(QRect(260, 90, 91, 16));
        fRunFileName_lineEdit = new QLineEdit(fRunHeaderInfo_groupBox);
        fRunFileName_lineEdit->setObjectName("fRunFileName_lineEdit");
        fRunFileName_lineEdit->setGeometry(QRect(120, 26, 311, 26));
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
        fFileFormat_comboBox->setGeometry(QRect(336, 86, 91, 25));
        fRequiredEntries_groupBox = new QGroupBox(PGetAsymmetryRunBlockDialog);
        fRequiredEntries_groupBox->setObjectName("fRequiredEntries_groupBox");
        fRequiredEntries_groupBox->setGeometry(QRect(10, 120, 441, 241));
        fMap_label = new QLabel(fRequiredEntries_groupBox);
        fMap_label->setObjectName("fMap_label");
        fMap_label->setGeometry(QRect(20, 30, 56, 16));
        fForwardHistoNo_label = new QLabel(fRequiredEntries_groupBox);
        fForwardHistoNo_label->setObjectName("fForwardHistoNo_label");
        fForwardHistoNo_label->setGeometry(QRect(20, 60, 111, 16));
        fBackwardHistoNo_label = new QLabel(fRequiredEntries_groupBox);
        fBackwardHistoNo_label->setObjectName("fBackwardHistoNo_label");
        fBackwardHistoNo_label->setGeometry(QRect(240, 60, 121, 16));
        fDataRange_label = new QLabel(fRequiredEntries_groupBox);
        fDataRange_label->setObjectName("fDataRange_label");
        fDataRange_label->setGeometry(QRect(20, 90, 81, 16));
        fBackgroundFix_label = new QLabel(fRequiredEntries_groupBox);
        fBackgroundFix_label->setObjectName("fBackgroundFix_label");
        fBackgroundFix_label->setGeometry(QRect(20, 120, 111, 16));
        fOr_label = new QLabel(fRequiredEntries_groupBox);
        fOr_label->setObjectName("fOr_label");
        fOr_label->setGeometry(QRect(310, 120, 31, 16));
        QFont font;
        font.setBold(true);
        fOr_label->setFont(font);
        fBackgroundRange_label = new QLabel(fRequiredEntries_groupBox);
        fBackgroundRange_label->setObjectName("fBackgroundRange_label");
        fBackgroundRange_label->setGeometry(QRect(20, 150, 121, 16));
        fFitRange_label = new QLabel(fRequiredEntries_groupBox);
        fFitRange_label->setObjectName("fFitRange_label");
        fFitRange_label->setGeometry(QRect(20, 180, 71, 16));
        fMap_lineEdit = new QLineEdit(fRequiredEntries_groupBox);
        fMap_lineEdit->setObjectName("fMap_lineEdit");
        fMap_lineEdit->setGeometry(QRect(60, 26, 371, 26));
        fForwardHistoNo_lineEdit = new QLineEdit(fRequiredEntries_groupBox);
        fForwardHistoNo_lineEdit->setObjectName("fForwardHistoNo_lineEdit");
        fForwardHistoNo_lineEdit->setGeometry(QRect(150, 56, 71, 26));
        fBackwardHistoNo_lineEdit = new QLineEdit(fRequiredEntries_groupBox);
        fBackwardHistoNo_lineEdit->setObjectName("fBackwardHistoNo_lineEdit");
        fBackwardHistoNo_lineEdit->setGeometry(QRect(360, 56, 71, 26));
        fDataForwardStart_lineEdit = new QLineEdit(fRequiredEntries_groupBox);
        fDataForwardStart_lineEdit->setObjectName("fDataForwardStart_lineEdit");
        fDataForwardStart_lineEdit->setGeometry(QRect(150, 86, 71, 26));
        fDataForwardEnd_lineEdit = new QLineEdit(fRequiredEntries_groupBox);
        fDataForwardEnd_lineEdit->setObjectName("fDataForwardEnd_lineEdit");
        fDataForwardEnd_lineEdit->setGeometry(QRect(220, 86, 71, 26));
        fDataBackwardStart_lineEdit = new QLineEdit(fRequiredEntries_groupBox);
        fDataBackwardStart_lineEdit->setObjectName("fDataBackwardStart_lineEdit");
        fDataBackwardStart_lineEdit->setGeometry(QRect(290, 86, 71, 26));
        fDataBackwardEnd_lineEdit = new QLineEdit(fRequiredEntries_groupBox);
        fDataBackwardEnd_lineEdit->setObjectName("fDataBackwardEnd_lineEdit");
        fDataBackwardEnd_lineEdit->setGeometry(QRect(360, 86, 71, 26));
        fBackgroundForwardFix_lineEdit = new QLineEdit(fRequiredEntries_groupBox);
        fBackgroundForwardFix_lineEdit->setObjectName("fBackgroundForwardFix_lineEdit");
        fBackgroundForwardFix_lineEdit->setGeometry(QRect(150, 116, 71, 26));
        fBackgroundBackwardFix_lineEdit = new QLineEdit(fRequiredEntries_groupBox);
        fBackgroundBackwardFix_lineEdit->setObjectName("fBackgroundBackwardFix_lineEdit");
        fBackgroundBackwardFix_lineEdit->setGeometry(QRect(220, 116, 71, 26));
        fBackgroundForwardStart_lineEdit = new QLineEdit(fRequiredEntries_groupBox);
        fBackgroundForwardStart_lineEdit->setObjectName("fBackgroundForwardStart_lineEdit");
        fBackgroundForwardStart_lineEdit->setGeometry(QRect(150, 146, 71, 26));
        fBackgroundForwardEnd_lineEdit = new QLineEdit(fRequiredEntries_groupBox);
        fBackgroundForwardEnd_lineEdit->setObjectName("fBackgroundForwardEnd_lineEdit");
        fBackgroundForwardEnd_lineEdit->setGeometry(QRect(220, 146, 71, 26));
        fBackgroundBackwardStart_lineEdit = new QLineEdit(fRequiredEntries_groupBox);
        fBackgroundBackwardStart_lineEdit->setObjectName("fBackgroundBackwardStart_lineEdit");
        fBackgroundBackwardStart_lineEdit->setGeometry(QRect(290, 146, 71, 26));
        fBackgroundBackwardEnd_lineEdit = new QLineEdit(fRequiredEntries_groupBox);
        fBackgroundBackwardEnd_lineEdit->setObjectName("fBackgroundBackwardEnd_lineEdit");
        fBackgroundBackwardEnd_lineEdit->setGeometry(QRect(360, 146, 71, 26));
        fFitRangeStart_lineEdit = new QLineEdit(fRequiredEntries_groupBox);
        fFitRangeStart_lineEdit->setObjectName("fFitRangeStart_lineEdit");
        fFitRangeStart_lineEdit->setGeometry(QRect(150, 176, 71, 26));
        fFitRangeEnd_lineEdit = new QLineEdit(fRequiredEntries_groupBox);
        fFitRangeEnd_lineEdit->setObjectName("fFitRangeEnd_lineEdit");
        fFitRangeEnd_lineEdit->setGeometry(QRect(220, 176, 71, 26));
        fPacking_lineEdit = new QLineEdit(fRequiredEntries_groupBox);
        fPacking_lineEdit->setObjectName("fPacking_lineEdit");
        fPacking_lineEdit->setGeometry(QRect(150, 206, 71, 26));
        fPacking_textLabel = new QLabel(fRequiredEntries_groupBox);
        fPacking_textLabel->setObjectName("fPacking_textLabel");
        fPacking_textLabel->setGeometry(QRect(20, 210, 111, 16));
        fOptionalEntries_groupBox = new QGroupBox(PGetAsymmetryRunBlockDialog);
        fOptionalEntries_groupBox->setObjectName("fOptionalEntries_groupBox");
        fOptionalEntries_groupBox->setGeometry(QRect(10, 360, 441, 91));
        fAplha_textLabel = new QLabel(fOptionalEntries_groupBox);
        fAplha_textLabel->setObjectName("fAplha_textLabel");
        fAplha_textLabel->setGeometry(QRect(20, 30, 131, 16));
        fBeta_label = new QLabel(fOptionalEntries_groupBox);
        fBeta_label->setObjectName("fBeta_label");
        fBeta_label->setGeometry(QRect(240, 30, 121, 16));
        fT0_label = new QLabel(fOptionalEntries_groupBox);
        fT0_label->setObjectName("fT0_label");
        fT0_label->setGeometry(QRect(20, 60, 241, 16));
        fAlpha_lineEdit = new QLineEdit(fOptionalEntries_groupBox);
        fAlpha_lineEdit->setObjectName("fAlpha_lineEdit");
        fAlpha_lineEdit->setGeometry(QRect(150, 26, 71, 26));
        fBeta_lineEdit = new QLineEdit(fOptionalEntries_groupBox);
        fBeta_lineEdit->setObjectName("fBeta_lineEdit");
        fBeta_lineEdit->setGeometry(QRect(360, 26, 71, 26));
        fT0Forward_lineEdit = new QLineEdit(fOptionalEntries_groupBox);
        fT0Forward_lineEdit->setObjectName("fT0Forward_lineEdit");
        fT0Forward_lineEdit->setGeometry(QRect(290, 56, 71, 26));
        fT0Backward_lineEdit = new QLineEdit(fOptionalEntries_groupBox);
        fT0Backward_lineEdit->setObjectName("fT0Backward_lineEdit");
        fT0Backward_lineEdit->setGeometry(QRect(360, 56, 71, 26));
        layoutWidget = new QWidget(PGetAsymmetryRunBlockDialog);
        layoutWidget->setObjectName("layoutWidget");
        layoutWidget->setGeometry(QRect(10, 460, 441, 39));
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


        retranslateUi(PGetAsymmetryRunBlockDialog);
        QObject::connect(fCancel_pushButton, &QPushButton::clicked, PGetAsymmetryRunBlockDialog, qOverload<>(&QDialog::reject));
        QObject::connect(fOk_pushButton, &QPushButton::clicked, PGetAsymmetryRunBlockDialog, qOverload<>(&QDialog::accept));
        QObject::connect(fHelp_pushButton, SIGNAL(clicked()), PGetAsymmetryRunBlockDialog, SLOT(helpContent()));

        QMetaObject::connectSlotsByName(PGetAsymmetryRunBlockDialog);
    } // setupUi

    void retranslateUi(QDialog *PGetAsymmetryRunBlockDialog)
    {
        PGetAsymmetryRunBlockDialog->setWindowTitle(QCoreApplication::translate("PGetAsymmetryRunBlockDialog", "Get Asymmetry Run Block Parameters", nullptr));
        fRunHeaderInfo_groupBox->setTitle(QCoreApplication::translate("PGetAsymmetryRunBlockDialog", "Run Header Info", nullptr));
        fRunFileName_label->setText(QCoreApplication::translate("PGetAsymmetryRunBlockDialog", "Run File Name", nullptr));
        fBeamline_label->setText(QCoreApplication::translate("PGetAsymmetryRunBlockDialog", "Beamline", nullptr));
        fInstitute_label->setText(QCoreApplication::translate("PGetAsymmetryRunBlockDialog", "Institute", nullptr));
        fFileFormat_label->setText(QCoreApplication::translate("PGetAsymmetryRunBlockDialog", "File Format", nullptr));
        fInstitute_comboBox->setItemText(0, QCoreApplication::translate("PGetAsymmetryRunBlockDialog", "PSI", nullptr));
        fInstitute_comboBox->setItemText(1, QCoreApplication::translate("PGetAsymmetryRunBlockDialog", "RAL", nullptr));
        fInstitute_comboBox->setItemText(2, QCoreApplication::translate("PGetAsymmetryRunBlockDialog", "TRIUMF", nullptr));
        fInstitute_comboBox->setItemText(3, QCoreApplication::translate("PGetAsymmetryRunBlockDialog", "JPARC", nullptr));

        fFileFormat_comboBox->setItemText(0, QCoreApplication::translate("PGetAsymmetryRunBlockDialog", "NeXuS", nullptr));
        fFileFormat_comboBox->setItemText(1, QCoreApplication::translate("PGetAsymmetryRunBlockDialog", "ROOT-NPP", nullptr));
        fFileFormat_comboBox->setItemText(2, QCoreApplication::translate("PGetAsymmetryRunBlockDialog", "ROOT-PPC", nullptr));
        fFileFormat_comboBox->setItemText(3, QCoreApplication::translate("PGetAsymmetryRunBlockDialog", "PSI-BIN", nullptr));
        fFileFormat_comboBox->setItemText(4, QCoreApplication::translate("PGetAsymmetryRunBlockDialog", "PSI-MDU", nullptr));
        fFileFormat_comboBox->setItemText(5, QCoreApplication::translate("PGetAsymmetryRunBlockDialog", "MUD", nullptr));
        fFileFormat_comboBox->setItemText(6, QCoreApplication::translate("PGetAsymmetryRunBlockDialog", "MDU-ASCII", nullptr));
        fFileFormat_comboBox->setItemText(7, QCoreApplication::translate("PGetAsymmetryRunBlockDialog", "ASCII", nullptr));
        fFileFormat_comboBox->setItemText(8, QCoreApplication::translate("PGetAsymmetryRunBlockDialog", "DB", nullptr));

        fRequiredEntries_groupBox->setTitle(QCoreApplication::translate("PGetAsymmetryRunBlockDialog", "Required Entries", nullptr));
        fMap_label->setText(QCoreApplication::translate("PGetAsymmetryRunBlockDialog", "Map", nullptr));
        fForwardHistoNo_label->setText(QCoreApplication::translate("PGetAsymmetryRunBlockDialog", "Forward Histo No", nullptr));
        fBackwardHistoNo_label->setText(QCoreApplication::translate("PGetAsymmetryRunBlockDialog", "Backward Histo No", nullptr));
        fDataRange_label->setText(QCoreApplication::translate("PGetAsymmetryRunBlockDialog", "Data  Range", nullptr));
        fBackgroundFix_label->setText(QCoreApplication::translate("PGetAsymmetryRunBlockDialog", "Background Fix", nullptr));
        fOr_label->setText(QCoreApplication::translate("PGetAsymmetryRunBlockDialog", "OR", nullptr));
        fBackgroundRange_label->setText(QCoreApplication::translate("PGetAsymmetryRunBlockDialog", "Background Range", nullptr));
        fFitRange_label->setText(QCoreApplication::translate("PGetAsymmetryRunBlockDialog", "Fit Range", nullptr));
        fMap_lineEdit->setText(QCoreApplication::translate("PGetAsymmetryRunBlockDialog", "0  0  0  0  0  0  0  0  0", nullptr));
        fPacking_lineEdit->setText(QCoreApplication::translate("PGetAsymmetryRunBlockDialog", "1", nullptr));
        fPacking_textLabel->setText(QCoreApplication::translate("PGetAsymmetryRunBlockDialog", "Packing/Binning", nullptr));
        fOptionalEntries_groupBox->setTitle(QCoreApplication::translate("PGetAsymmetryRunBlockDialog", "Optional Entries", nullptr));
        fAplha_textLabel->setText(QCoreApplication::translate("PGetAsymmetryRunBlockDialog", "Alpha (default = 1.0)", nullptr));
        fBeta_label->setText(QCoreApplication::translate("PGetAsymmetryRunBlockDialog", "Beta (default = 1.0)", nullptr));
        fT0_label->setText(QCoreApplication::translate("PGetAsymmetryRunBlockDialog", "t0 (reqired if not given in the data file)", nullptr));
        fAlpha_lineEdit->setText(QString());
        fBeta_lineEdit->setText(QString());
        fHelp_pushButton->setText(QCoreApplication::translate("PGetAsymmetryRunBlockDialog", "&Help", nullptr));
        fOk_pushButton->setText(QCoreApplication::translate("PGetAsymmetryRunBlockDialog", "&OK", nullptr));
        fCancel_pushButton->setText(QCoreApplication::translate("PGetAsymmetryRunBlockDialog", "&Cancel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class PGetAsymmetryRunBlockDialog: public Ui_PGetAsymmetryRunBlockDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PGETASYMMETRYRUNBLOCKDIALOG_H
