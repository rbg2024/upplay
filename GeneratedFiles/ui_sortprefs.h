/********************************************************************************
** Form generated from reading UI file 'sortprefs.ui'
**
** Created by: Qt User Interface Compiler version 5.5.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SORTPREFS_H
#define UI_SORTPREFS_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_SortprefsDLG
{
public:
    QVBoxLayout *verticalLayout_2;
    QVBoxLayout *verticalLayout;
    QRadioButton *noSortRB;
    QRadioButton *minimfnRB;
    QRadioButton *sortRB;
    QListWidget *critsLW;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *SortprefsDLG)
    {
        if (SortprefsDLG->objectName().isEmpty())
            SortprefsDLG->setObjectName(QStringLiteral("SortprefsDLG"));
        SortprefsDLG->resize(430, 320);
        verticalLayout_2 = new QVBoxLayout(SortprefsDLG);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        noSortRB = new QRadioButton(SortprefsDLG);
        noSortRB->setObjectName(QStringLiteral("noSortRB"));

        verticalLayout->addWidget(noSortRB);

        minimfnRB = new QRadioButton(SortprefsDLG);
        minimfnRB->setObjectName(QStringLiteral("minimfnRB"));

        verticalLayout->addWidget(minimfnRB);

        sortRB = new QRadioButton(SortprefsDLG);
        sortRB->setObjectName(QStringLiteral("sortRB"));

        verticalLayout->addWidget(sortRB);

        critsLW = new QListWidget(SortprefsDLG);
        critsLW->setObjectName(QStringLiteral("critsLW"));

        verticalLayout->addWidget(critsLW);


        verticalLayout_2->addLayout(verticalLayout);

        buttonBox = new QDialogButtonBox(SortprefsDLG);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout_2->addWidget(buttonBox);


        retranslateUi(SortprefsDLG);
        QObject::connect(buttonBox, SIGNAL(accepted()), SortprefsDLG, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), SortprefsDLG, SLOT(reject()));

        QMetaObject::connectSlotsByName(SortprefsDLG);
    } // setupUi

    void retranslateUi(QDialog *SortprefsDLG)
    {
        SortprefsDLG->setWindowTitle(QApplication::translate("SortprefsDLG", "Directory sorting", 0));
        noSortRB->setText(QApplication::translate("SortprefsDLG", "Keep Content Directory order", 0));
        minimfnRB->setText(QApplication::translate("SortprefsDLG", "Use file name ordering inside folder hierarchy (Minimserver only)", 0));
        sortRB->setText(QApplication::translate("SortprefsDLG", "Sort by fields (use drag and drop to reorder)", 0));
    } // retranslateUi

};

namespace Ui {
    class SortprefsDLG: public Ui_SortprefsDLG {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SORTPREFS_H
