/********************************************************************************
** Form generated from reading UI file 'PReplaceDialog.ui'
**
** Created by: Qt User Interface Compiler version 6.4.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PREPLACEDIALOG_H
#define UI_PREPLACEDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_PReplaceDialog
{
public:
    QGroupBox *fFind_groupBox;
    QLabel *fFind_label;
    QComboBox *fFind_comboBox;
    QPushButton *fReplace_pushButton;
    QPushButton *fClose_pushButton;
    QGroupBox *fOptions_groupBox;
    QCheckBox *fCaseSensitive_checkBox;
    QCheckBox *fWholeWordsOnly_checkBox;
    QCheckBox *fFromCursor_checkBox;
    QCheckBox *fFindBackwards_checkBox;
    QCheckBox *fSelectedText_checkBox;
    QCheckBox *fPromptOnReplace_checkBox;
    QFrame *line;
    QGroupBox *fReplacement_groupBox;
    QLabel *fReplace_textLabel;
    QComboBox *fReplacementText_comboBox;

    void setupUi(QDialog *PReplaceDialog)
    {
        if (PReplaceDialog->objectName().isEmpty())
            PReplaceDialog->setObjectName("PReplaceDialog");
        PReplaceDialog->resize(345, 315);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/images/musrfit.xpm"), QSize(), QIcon::Normal, QIcon::Off);
        PReplaceDialog->setWindowIcon(icon);
        fFind_groupBox = new QGroupBox(PReplaceDialog);
        fFind_groupBox->setObjectName("fFind_groupBox");
        fFind_groupBox->setGeometry(QRect(0, 0, 341, 81));
        fFind_label = new QLabel(fFind_groupBox);
        fFind_label->setObjectName("fFind_label");
        fFind_label->setGeometry(QRect(10, 20, 91, 16));
        fFind_comboBox = new QComboBox(fFind_groupBox);
        fFind_comboBox->setObjectName("fFind_comboBox");
        fFind_comboBox->setGeometry(QRect(10, 40, 321, 31));
        fFind_comboBox->setEditable(true);
        fReplace_pushButton = new QPushButton(PReplaceDialog);
        fReplace_pushButton->setObjectName("fReplace_pushButton");
        fReplace_pushButton->setGeometry(QRect(190, 290, 71, 25));
        fClose_pushButton = new QPushButton(PReplaceDialog);
        fClose_pushButton->setObjectName("fClose_pushButton");
        fClose_pushButton->setGeometry(QRect(270, 290, 71, 25));
        fOptions_groupBox = new QGroupBox(PReplaceDialog);
        fOptions_groupBox->setObjectName("fOptions_groupBox");
        fOptions_groupBox->setGeometry(QRect(0, 160, 341, 121));
        fOptions_groupBox->setAlignment(Qt::AlignBottom|Qt::AlignLeading|Qt::AlignLeft);
        fCaseSensitive_checkBox = new QCheckBox(fOptions_groupBox);
        fCaseSensitive_checkBox->setObjectName("fCaseSensitive_checkBox");
        fCaseSensitive_checkBox->setGeometry(QRect(10, 30, 141, 23));
        fWholeWordsOnly_checkBox = new QCheckBox(fOptions_groupBox);
        fWholeWordsOnly_checkBox->setObjectName("fWholeWordsOnly_checkBox");
        fWholeWordsOnly_checkBox->setGeometry(QRect(10, 60, 141, 23));
        fFromCursor_checkBox = new QCheckBox(fOptions_groupBox);
        fFromCursor_checkBox->setObjectName("fFromCursor_checkBox");
        fFromCursor_checkBox->setGeometry(QRect(10, 90, 141, 23));
        fFindBackwards_checkBox = new QCheckBox(fOptions_groupBox);
        fFindBackwards_checkBox->setObjectName("fFindBackwards_checkBox");
        fFindBackwards_checkBox->setGeometry(QRect(180, 30, 141, 23));
        fSelectedText_checkBox = new QCheckBox(fOptions_groupBox);
        fSelectedText_checkBox->setObjectName("fSelectedText_checkBox");
        fSelectedText_checkBox->setGeometry(QRect(180, 60, 141, 23));
        fPromptOnReplace_checkBox = new QCheckBox(fOptions_groupBox);
        fPromptOnReplace_checkBox->setObjectName("fPromptOnReplace_checkBox");
        fPromptOnReplace_checkBox->setGeometry(QRect(180, 90, 151, 23));
        line = new QFrame(PReplaceDialog);
        line->setObjectName("line");
        line->setGeometry(QRect(0, 280, 351, 16));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        fReplacement_groupBox = new QGroupBox(PReplaceDialog);
        fReplacement_groupBox->setObjectName("fReplacement_groupBox");
        fReplacement_groupBox->setGeometry(QRect(0, 80, 341, 81));
        fReplace_textLabel = new QLabel(fReplacement_groupBox);
        fReplace_textLabel->setObjectName("fReplace_textLabel");
        fReplace_textLabel->setGeometry(QRect(10, 20, 111, 16));
        fReplacementText_comboBox = new QComboBox(fReplacement_groupBox);
        fReplacementText_comboBox->setObjectName("fReplacementText_comboBox");
        fReplacementText_comboBox->setGeometry(QRect(10, 40, 321, 31));
        fReplacementText_comboBox->setEditable(true);
        QWidget::setTabOrder(fFind_comboBox, fReplacementText_comboBox);
        QWidget::setTabOrder(fReplacementText_comboBox, fCaseSensitive_checkBox);
        QWidget::setTabOrder(fCaseSensitive_checkBox, fWholeWordsOnly_checkBox);
        QWidget::setTabOrder(fWholeWordsOnly_checkBox, fFromCursor_checkBox);
        QWidget::setTabOrder(fFromCursor_checkBox, fFindBackwards_checkBox);
        QWidget::setTabOrder(fFindBackwards_checkBox, fSelectedText_checkBox);
        QWidget::setTabOrder(fSelectedText_checkBox, fPromptOnReplace_checkBox);
        QWidget::setTabOrder(fPromptOnReplace_checkBox, fReplace_pushButton);
        QWidget::setTabOrder(fReplace_pushButton, fClose_pushButton);

        retranslateUi(PReplaceDialog);
        QObject::connect(fClose_pushButton, &QPushButton::clicked, PReplaceDialog, qOverload<>(&QDialog::reject));
        QObject::connect(fReplace_pushButton, &QPushButton::clicked, PReplaceDialog, qOverload<>(&QDialog::accept));
        QObject::connect(fFind_comboBox, SIGNAL(editTextChanged(QString)), PReplaceDialog, SLOT(onFindTextAvailable(QString)));

        QMetaObject::connectSlotsByName(PReplaceDialog);
    } // setupUi

    void retranslateUi(QDialog *PReplaceDialog)
    {
        PReplaceDialog->setWindowTitle(QCoreApplication::translate("PReplaceDialog", "Replace Text - musredit", nullptr));
        fFind_groupBox->setTitle(QCoreApplication::translate("PReplaceDialog", "Find", nullptr));
        fFind_label->setText(QCoreApplication::translate("PReplaceDialog", "Text to find:", nullptr));
        fReplace_pushButton->setText(QCoreApplication::translate("PReplaceDialog", "&Replace", nullptr));
        fClose_pushButton->setText(QCoreApplication::translate("PReplaceDialog", "&Close", nullptr));
        fOptions_groupBox->setTitle(QCoreApplication::translate("PReplaceDialog", "Options", nullptr));
        fCaseSensitive_checkBox->setText(QCoreApplication::translate("PReplaceDialog", "C&ase Sensitive", nullptr));
        fWholeWordsOnly_checkBox->setText(QCoreApplication::translate("PReplaceDialog", "&Whole words only", nullptr));
        fFromCursor_checkBox->setText(QCoreApplication::translate("PReplaceDialog", "From c&ursor", nullptr));
        fFindBackwards_checkBox->setText(QCoreApplication::translate("PReplaceDialog", "Find &backwards", nullptr));
        fSelectedText_checkBox->setText(QCoreApplication::translate("PReplaceDialog", "&Selected text", nullptr));
        fPromptOnReplace_checkBox->setText(QCoreApplication::translate("PReplaceDialog", "&Prompt on replace", nullptr));
        fReplacement_groupBox->setTitle(QCoreApplication::translate("PReplaceDialog", "Replace With", nullptr));
        fReplace_textLabel->setText(QCoreApplication::translate("PReplaceDialog", "Replacement text:", nullptr));
    } // retranslateUi

};

namespace Ui {
    class PReplaceDialog: public Ui_PReplaceDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PREPLACEDIALOG_H
