/********************************************************************************
** Form generated from reading UI file 'GUI_PlaylistEntryBig.ui'
**
** Created by: Qt User Interface Compiler version 5.5.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GUI_PLAYLISTENTRYBIG_H
#define UI_GUI_PLAYLISTENTRYBIG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpacerItem>

QT_BEGIN_NAMESPACE

class Ui_PlaylistEntryBig
{
public:
    QGridLayout *gridLayout;
    QLabel *lab_title;
    QLabel *lab_artist;
    QLabel *lab_album;
    QSpacerItem *horizontalSpacer;
    QLabel *lab_time;

    void setupUi(QFrame *PlaylistEntryBig)
    {
        if (PlaylistEntryBig->objectName().isEmpty())
            PlaylistEntryBig->setObjectName(QStringLiteral("PlaylistEntryBig"));
        PlaylistEntryBig->resize(370, 25);
        QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(PlaylistEntryBig->sizePolicy().hasHeightForWidth());
        PlaylistEntryBig->setSizePolicy(sizePolicy);
        PlaylistEntryBig->setMinimumSize(QSize(0, 0));
        PlaylistEntryBig->setMaximumSize(QSize(800, 500));
        QFont font;
        font.setPointSize(8);
        PlaylistEntryBig->setFont(font);
        PlaylistEntryBig->setFrameShape(QFrame::Box);
        PlaylistEntryBig->setFrameShadow(QFrame::Plain);
        PlaylistEntryBig->setLineWidth(0);
        PlaylistEntryBig->setMidLineWidth(0);
        gridLayout = new QGridLayout(PlaylistEntryBig);
        gridLayout->setSpacing(2);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(2, 0, 10, 0);
        lab_title = new QLabel(PlaylistEntryBig);
        lab_title->setObjectName(QStringLiteral("lab_title"));
        lab_title->setMinimumSize(QSize(20, 10));
        lab_title->setMaximumSize(QSize(5000, 18));
        QFont font1;
        font1.setPointSize(8);
        font1.setBold(true);
        font1.setWeight(75);
        lab_title->setFont(font1);
        lab_title->setLineWidth(0);
        lab_title->setAlignment(Qt::AlignBottom|Qt::AlignLeading|Qt::AlignLeft);
        lab_title->setWordWrap(false);

        gridLayout->addWidget(lab_title, 0, 0, 1, 5);

        lab_artist = new QLabel(PlaylistEntryBig);
        lab_artist->setObjectName(QStringLiteral("lab_artist"));
        QSizePolicy sizePolicy1(QSizePolicy::Maximum, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(lab_artist->sizePolicy().hasHeightForWidth());
        lab_artist->setSizePolicy(sizePolicy1);
        lab_artist->setMinimumSize(QSize(0, 0));
        lab_artist->setMaximumSize(QSize(16777215, 10));
        QFont font2;
        font2.setPointSize(7);
        font2.setBold(false);
        font2.setItalic(false);
        font2.setWeight(50);
        lab_artist->setFont(font2);

        gridLayout->addWidget(lab_artist, 1, 0, 1, 1);

        lab_album = new QLabel(PlaylistEntryBig);
        lab_album->setObjectName(QStringLiteral("lab_album"));
        sizePolicy1.setHeightForWidth(lab_album->sizePolicy().hasHeightForWidth());
        lab_album->setSizePolicy(sizePolicy1);
        lab_album->setMinimumSize(QSize(0, 0));
        lab_album->setMaximumSize(QSize(16777215, 10));
        lab_album->setFont(font2);

        gridLayout->addWidget(lab_album, 1, 1, 1, 1);

        horizontalSpacer = new QSpacerItem(40, 10, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer, 1, 2, 1, 2);

        lab_time = new QLabel(PlaylistEntryBig);
        lab_time->setObjectName(QStringLiteral("lab_time"));
        sizePolicy1.setHeightForWidth(lab_time->sizePolicy().hasHeightForWidth());
        lab_time->setSizePolicy(sizePolicy1);
        lab_time->setMinimumSize(QSize(0, 0));
        lab_time->setMaximumSize(QSize(16777215, 10));
        lab_time->setFont(font2);

        gridLayout->addWidget(lab_time, 1, 4, 1, 1);


        retranslateUi(PlaylistEntryBig);

        QMetaObject::connectSlotsByName(PlaylistEntryBig);
    } // setupUi

    void retranslateUi(QFrame *PlaylistEntryBig)
    {
        PlaylistEntryBig->setWindowTitle(QApplication::translate("PlaylistEntryBig", "PlaylistEntryBig", 0));
        lab_title->setText(QApplication::translate("PlaylistEntryBig", "Title", 0));
        lab_artist->setText(QApplication::translate("PlaylistEntryBig", "Artist", 0));
        lab_album->setText(QApplication::translate("PlaylistEntryBig", "(Album)", 0));
        lab_time->setText(QApplication::translate("PlaylistEntryBig", "Time", 0));
    } // retranslateUi

};

namespace Ui {
    class PlaylistEntryBig: public Ui_PlaylistEntryBig {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GUI_PLAYLISTENTRYBIG_H
