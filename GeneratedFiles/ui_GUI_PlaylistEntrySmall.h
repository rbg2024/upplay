/********************************************************************************
** Form generated from reading UI file 'GUI_PlaylistEntrySmall.ui'
**
** Created by: Qt User Interface Compiler version 5.5.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GUI_PLAYLISTENTRYSMALL_H
#define UI_GUI_PLAYLISTENTRYSMALL_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>

QT_BEGIN_NAMESPACE

class Ui_PlaylistEntrySmall
{
public:
    QHBoxLayout *horizontalLayout;
    QLabel *lab_title;
    QLabel *lab_artist;
    QLabel *lab_time;

    void setupUi(QFrame *PlaylistEntrySmall)
    {
        if (PlaylistEntrySmall->objectName().isEmpty())
            PlaylistEntrySmall->setObjectName(QStringLiteral("PlaylistEntrySmall"));
        PlaylistEntrySmall->resize(285, 10);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(PlaylistEntrySmall->sizePolicy().hasHeightForWidth());
        PlaylistEntrySmall->setSizePolicy(sizePolicy);
        PlaylistEntrySmall->setStyleSheet(QStringLiteral("background: transparent;"));
        PlaylistEntrySmall->setFrameShape(QFrame::NoFrame);
        PlaylistEntrySmall->setFrameShadow(QFrame::Raised);
        PlaylistEntrySmall->setLineWidth(0);
        horizontalLayout = new QHBoxLayout(PlaylistEntrySmall);
        horizontalLayout->setSpacing(5);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(2, 0, 10, 0);
        lab_title = new QLabel(PlaylistEntrySmall);
        lab_title->setObjectName(QStringLiteral("lab_title"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(lab_title->sizePolicy().hasHeightForWidth());
        lab_title->setSizePolicy(sizePolicy1);
        lab_title->setMinimumSize(QSize(0, 10));
        lab_title->setMaximumSize(QSize(546, 100));
        QFont font;
        font.setPointSize(8);
        font.setBold(true);
        font.setWeight(75);
        lab_title->setFont(font);
        lab_title->setLineWidth(0);

        horizontalLayout->addWidget(lab_title);

        lab_artist = new QLabel(PlaylistEntrySmall);
        lab_artist->setObjectName(QStringLiteral("lab_artist"));
        QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(lab_artist->sizePolicy().hasHeightForWidth());
        lab_artist->setSizePolicy(sizePolicy2);
        lab_artist->setMinimumSize(QSize(0, 10));
        lab_artist->setMaximumSize(QSize(500, 100));
        QFont font1;
        font1.setPointSize(7);
        lab_artist->setFont(font1);
        lab_artist->setLineWidth(0);
        lab_artist->setIndent(0);

        horizontalLayout->addWidget(lab_artist);

        lab_time = new QLabel(PlaylistEntrySmall);
        lab_time->setObjectName(QStringLiteral("lab_time"));
        sizePolicy1.setHeightForWidth(lab_time->sizePolicy().hasHeightForWidth());
        lab_time->setSizePolicy(sizePolicy1);
        lab_time->setMinimumSize(QSize(30, 10));
        lab_time->setMaximumSize(QSize(40, 100));
        lab_time->setFont(font1);
        lab_time->setLineWidth(0);
        lab_time->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        lab_time->setIndent(0);

        horizontalLayout->addWidget(lab_time);


        retranslateUi(PlaylistEntrySmall);

        QMetaObject::connectSlotsByName(PlaylistEntrySmall);
    } // setupUi

    void retranslateUi(QFrame *PlaylistEntrySmall)
    {
        PlaylistEntrySmall->setWindowTitle(QApplication::translate("PlaylistEntrySmall", "PlaylistEntrySmall", 0));
        lab_title->setText(QApplication::translate("PlaylistEntrySmall", "Title", 0));
        lab_artist->setText(QApplication::translate("PlaylistEntrySmall", "Artist", 0));
        lab_time->setText(QApplication::translate("PlaylistEntrySmall", "Time", 0));
    } // retranslateUi

};

namespace Ui {
    class PlaylistEntrySmall: public Ui_PlaylistEntrySmall {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GUI_PLAYLISTENTRYSMALL_H
