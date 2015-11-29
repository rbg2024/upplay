/********************************************************************************
** Form generated from reading UI file 'mdatawidget.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MDATAWIDGET_H
#define UI_MDATAWIDGET_H

#include <QLocale>
#include <QVariant>
#include <QAction>
#include <QApplication>
#include <QButtonGroup>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>

QT_BEGIN_NAMESPACE

class Ui_MDataWidget
{
public:
    QHBoxLayout *horizontalLayout_2;
    QLayout *actualLayout;
    QLabel *lab_title;
    QLabel *lab_artist;
    QLabel *lab_album;

    void setupUi(QWidget *MDataWidget, bool horiz)
    {
        if (MDataWidget->objectName().isEmpty())
            MDataWidget->setObjectName(QString::fromUtf8("MDataWidget"));
        MDataWidget->resize(300, 40);
        
        horizontalLayout_2 = new QHBoxLayout(MDataWidget);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));

        int titlehmin = 20;
        int otherhmin = 20;
        if (horiz) {
            actualLayout = new QHBoxLayout();
        } else {
            titlehmin = 50;
            otherhmin = 30;
            actualLayout = new QVBoxLayout();
        }

        actualLayout->setObjectName(QString::fromUtf8("actualLayout"));

        lab_title = new QLabel(MDataWidget);
        lab_title->setObjectName(QString::fromUtf8("lab_title"));
        lab_title->setMinimumSize(QSize(0, titlehmin));
        lab_title->setMaximumSize(QSize(16777215, 50));
        QFont font;
        font.setPointSize(10);
        font.setBold(true);
        font.setWeight(75);
        lab_title->setFont(font);
        lab_title->setStyleSheet(QString::fromUtf8(""));
        lab_title->setFrameShape(QFrame::NoFrame);
        lab_title->setFrameShadow(QFrame::Plain);
        lab_title->setInputMethodHints(Qt::ImhNone);
        lab_title->setText(QString::fromUtf8(""));
        lab_title->setTextFormat(Qt::AutoText);
        lab_title->setScaledContents(true);
        lab_title->setAlignment(Qt::AlignCenter);
        lab_title->setWordWrap(true);
        lab_title->setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::TextSelectableByMouse);

        actualLayout->addWidget(lab_title);

        lab_artist = new QLabel(MDataWidget);
        lab_artist->setObjectName(QString::fromUtf8("lab_artist"));
        lab_artist->setMinimumSize(QSize(0, otherhmin));
        lab_artist->setMaximumSize(QSize(16777215, 30));
        QFont font1;
        font1.setPointSize(6);
        font1.setStyleStrategy(QFont::PreferAntialias);
        lab_artist->setFont(font1);
        lab_artist->setStyleSheet(QString::fromUtf8(""));
        lab_artist->setFrameShape(QFrame::NoFrame);
        lab_artist->setFrameShadow(QFrame::Plain);
        lab_artist->setText(QString::fromUtf8(""));
        lab_artist->setTextFormat(Qt::AutoText);
        lab_artist->setAlignment(Qt::AlignCenter);
        lab_artist->setWordWrap(true);
        lab_artist->setOpenExternalLinks(false);

        actualLayout->addWidget(lab_artist);

        lab_album = new QLabel(MDataWidget);
        lab_album->setObjectName(QString::fromUtf8("lab_album"));
        lab_album->setMinimumSize(QSize(0, otherhmin));
        lab_album->setMaximumSize(QSize(16777215, 30));
        lab_album->setFont(font1);
        lab_album->setStyleSheet(QString::fromUtf8(""));
        lab_album->setText(QString::fromUtf8(""));
        lab_album->setAlignment(Qt::AlignCenter);
        lab_album->setWordWrap(true);

        actualLayout->addWidget(lab_album);


        horizontalLayout_2->addLayout(actualLayout);

        retranslateUi(MDataWidget);

        QMetaObject::connectSlotsByName(MDataWidget);
    } // setupUi

    void retranslateUi(QWidget *MDataWidget)
    {
        Q_UNUSED(MDataWidget);
    } // retranslateUi

};

namespace Ui {
    class MDataWidget: public Ui_MDataWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MDATAWIDGET_H
