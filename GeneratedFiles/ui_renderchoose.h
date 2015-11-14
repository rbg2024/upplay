/********************************************************************************
** Form generated from reading UI file 'renderchoose.ui'
**
** Created by: Qt User Interface Compiler version 5.5.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RENDERCHOOSE_H
#define UI_RENDERCHOOSE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_RenderChooseDLG
{
public:
    QVBoxLayout *verticalLayout_2;
    QVBoxLayout *verticalLayout;
    QListWidget *rndsLW;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QRadioButton *keepRB;
    QRadioButton *replRB;
    QRadioButton *apndRB;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *RenderChooseDLG)
    {
        if (RenderChooseDLG->objectName().isEmpty())
            RenderChooseDLG->setObjectName(QStringLiteral("RenderChooseDLG"));
        RenderChooseDLG->setWindowModality(Qt::ApplicationModal);
        RenderChooseDLG->resize(290, 209);
        verticalLayout_2 = new QVBoxLayout(RenderChooseDLG);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        rndsLW = new QListWidget(RenderChooseDLG);
        rndsLW->setObjectName(QStringLiteral("rndsLW"));

        verticalLayout->addWidget(rndsLW);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        label = new QLabel(RenderChooseDLG);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout->addWidget(label);

        keepRB = new QRadioButton(RenderChooseDLG);
        keepRB->setObjectName(QStringLiteral("keepRB"));
        keepRB->setChecked(true);

        horizontalLayout->addWidget(keepRB);

        replRB = new QRadioButton(RenderChooseDLG);
        replRB->setObjectName(QStringLiteral("replRB"));

        horizontalLayout->addWidget(replRB);

        apndRB = new QRadioButton(RenderChooseDLG);
        apndRB->setObjectName(QStringLiteral("apndRB"));
        apndRB->setAutoExclusive(true);

        horizontalLayout->addWidget(apndRB);


        verticalLayout->addLayout(horizontalLayout);


        verticalLayout_2->addLayout(verticalLayout);

        buttonBox = new QDialogButtonBox(RenderChooseDLG);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout_2->addWidget(buttonBox);


        retranslateUi(RenderChooseDLG);
        QObject::connect(buttonBox, SIGNAL(accepted()), RenderChooseDLG, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), RenderChooseDLG, SLOT(reject()));
        QObject::connect(rndsLW, SIGNAL(itemDoubleClicked(QListWidgetItem*)), RenderChooseDLG, SLOT(accept()));

        QMetaObject::connectSlotsByName(RenderChooseDLG);
    } // setupUi

    void retranslateUi(QDialog *RenderChooseDLG)
    {
        RenderChooseDLG->setWindowTitle(QApplication::translate("RenderChooseDLG", "Select Media Renderer for Upplay", 0));
        label->setText(QApplication::translate("RenderChooseDLG", "Playlist:", 0));
        keepRB->setText(QApplication::translate("RenderChooseDLG", "Keep", 0));
        replRB->setText(QApplication::translate("RenderChooseDLG", "Replace", 0));
        apndRB->setText(QApplication::translate("RenderChooseDLG", "Append", 0));
    } // retranslateUi

};

namespace Ui {
    class RenderChooseDLG: public Ui_RenderChooseDLG {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RENDERCHOOSE_H
