/********************************************************************************
** Form generated from reading UI file 'GUI_Playlist.ui'
**
** Created by: Qt User Interface Compiler version 5.5.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GUI_PLAYLIST_H
#define UI_GUI_PLAYLIST_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "GUI/playlist/view/PlaylistView.h"

QT_BEGIN_NAMESPACE

class Ui_Playlist_Window
{
public:
    QVBoxLayout *verticalLayout_2;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *lab_totalTime;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *btn_clear;
    PlaylistView *listView;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *btn_repAll;
    QPushButton *btn_shuffle;
    QSpacerItem *horizontalSpacer;
    QPushButton *btn_append;
    QPushButton *btn_replace;
    QPushButton *btn_playAdded;
    QSpacerItem *horizontalSpacer1;
    QPushButton *btn_numbers;

    void setupUi(QWidget *Playlist_Window)
    {
        if (Playlist_Window->objectName().isEmpty())
            Playlist_Window->setObjectName(QStringLiteral("Playlist_Window"));
        Playlist_Window->setWindowModality(Qt::NonModal);
        Playlist_Window->resize(439, 326);
        verticalLayout_2 = new QVBoxLayout(Playlist_Window);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(2);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        lab_totalTime = new QLabel(Playlist_Window);
        lab_totalTime->setObjectName(QStringLiteral("lab_totalTime"));
        lab_totalTime->setMinimumSize(QSize(0, 25));
        lab_totalTime->setMaximumSize(QSize(1000, 16777215));
        QFont font;
        font.setBold(true);
        font.setWeight(75);
        lab_totalTime->setFont(font);
        lab_totalTime->setText(QStringLiteral("0m 0s"));
        lab_totalTime->setAlignment(Qt::AlignBottom|Qt::AlignLeading|Qt::AlignLeft);

        horizontalLayout->addWidget(lab_totalTime);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);

        btn_clear = new QPushButton(Playlist_Window);
        btn_clear->setObjectName(QStringLiteral("btn_clear"));
        btn_clear->setMinimumSize(QSize(24, 24));
        btn_clear->setMaximumSize(QSize(26, 26));
        btn_clear->setFocusPolicy(Qt::NoFocus);
        QIcon icon;
        icon.addFile(QStringLiteral("../../../.designer/backup/icons/broom.png"), QSize(), QIcon::Normal, QIcon::Off);
        btn_clear->setIcon(icon);
        btn_clear->setIconSize(QSize(24, 24));
        btn_clear->setFlat(true);

        horizontalLayout->addWidget(btn_clear);


        verticalLayout->addLayout(horizontalLayout);

        listView = new PlaylistView(Playlist_Window);
        listView->setObjectName(QStringLiteral("listView"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(1);
        sizePolicy.setHeightForWidth(listView->sizePolicy().hasHeightForWidth());
        listView->setSizePolicy(sizePolicy);
        listView->setMinimumSize(QSize(100, 30));
        listView->setMaximumSize(QSize(167721, 16777215));
        listView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        listView->setTabKeyNavigation(true);
        listView->setAlternatingRowColors(true);
        listView->setSelectionMode(QAbstractItemView::ExtendedSelection);

        verticalLayout->addWidget(listView);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        btn_repAll = new QPushButton(Playlist_Window);
        btn_repAll->setObjectName(QStringLiteral("btn_repAll"));
        btn_repAll->setMinimumSize(QSize(28, 28));
        btn_repAll->setMaximumSize(QSize(28, 28));
        btn_repAll->setFocusPolicy(Qt::NoFocus);
        QIcon icon1;
        icon1.addFile(QStringLiteral("../../../icons/repAll.png"), QSize(), QIcon::Normal, QIcon::Off);
        btn_repAll->setIcon(icon1);
        btn_repAll->setIconSize(QSize(20, 20));
        btn_repAll->setCheckable(true);
        btn_repAll->setFlat(true);

        horizontalLayout_2->addWidget(btn_repAll);

        btn_shuffle = new QPushButton(Playlist_Window);
        btn_shuffle->setObjectName(QStringLiteral("btn_shuffle"));
        btn_shuffle->setMinimumSize(QSize(28, 28));
        btn_shuffle->setMaximumSize(QSize(28, 28));
        btn_shuffle->setFocusPolicy(Qt::NoFocus);
        QIcon icon2;
        icon2.addFile(QStringLiteral("../../../icons/shuffle.png"), QSize(), QIcon::Normal, QIcon::Off);
        btn_shuffle->setIcon(icon2);
        btn_shuffle->setIconSize(QSize(20, 20));
        btn_shuffle->setCheckable(true);
        btn_shuffle->setFlat(true);

        horizontalLayout_2->addWidget(btn_shuffle);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        btn_append = new QPushButton(Playlist_Window);
        btn_append->setObjectName(QStringLiteral("btn_append"));
        btn_append->setMinimumSize(QSize(28, 28));
        btn_append->setMaximumSize(QSize(28, 28));
        btn_append->setFocusPolicy(Qt::NoFocus);
        QIcon icon3;
        icon3.addFile(QStringLiteral("../../../.designer/backup/icons/append.png"), QSize(), QIcon::Normal, QIcon::Off);
        btn_append->setIcon(icon3);
        btn_append->setIconSize(QSize(20, 20));
        btn_append->setCheckable(true);
        btn_append->setFlat(true);

        horizontalLayout_2->addWidget(btn_append);

        btn_replace = new QPushButton(Playlist_Window);
        btn_replace->setObjectName(QStringLiteral("btn_replace"));
        btn_replace->setMinimumSize(QSize(28, 28));
        btn_replace->setMaximumSize(QSize(28, 28));
        btn_replace->setFocusPolicy(Qt::NoFocus);
        btn_replace->setIcon(icon);
        btn_replace->setIconSize(QSize(20, 20));
        btn_replace->setCheckable(true);
        btn_replace->setFlat(true);

        horizontalLayout_2->addWidget(btn_replace);

        btn_playAdded = new QPushButton(Playlist_Window);
        btn_playAdded->setObjectName(QStringLiteral("btn_playAdded"));
        btn_playAdded->setMinimumSize(QSize(28, 28));
        btn_playAdded->setMaximumSize(QSize(28, 28));
        btn_playAdded->setFocusPolicy(Qt::NoFocus);
        QIcon icon4;
        icon4.addFile(QStringLiteral("../../../.designer/backup/icons/dynamic.png"), QSize(), QIcon::Normal, QIcon::Off);
        btn_playAdded->setIcon(icon4);
        btn_playAdded->setIconSize(QSize(20, 20));
        btn_playAdded->setCheckable(true);
        btn_playAdded->setFlat(true);

        horizontalLayout_2->addWidget(btn_playAdded);

        horizontalSpacer1 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer1);

        btn_numbers = new QPushButton(Playlist_Window);
        btn_numbers->setObjectName(QStringLiteral("btn_numbers"));
        btn_numbers->setMinimumSize(QSize(28, 28));
        btn_numbers->setMaximumSize(QSize(28, 28));
        btn_numbers->setFocusPolicy(Qt::NoFocus);
        btn_numbers->setIconSize(QSize(20, 20));
        btn_numbers->setCheckable(true);
        btn_numbers->setFlat(true);

        horizontalLayout_2->addWidget(btn_numbers);


        verticalLayout->addLayout(horizontalLayout_2);

        verticalLayout->setStretch(1, 1);

        verticalLayout_2->addLayout(verticalLayout);


        retranslateUi(Playlist_Window);

        QMetaObject::connectSlotsByName(Playlist_Window);
    } // setupUi

    void retranslateUi(QWidget *Playlist_Window)
    {
#ifndef QT_NO_TOOLTIP
        Playlist_Window->setToolTip(QString());
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        btn_clear->setToolTip(QApplication::translate("Playlist_Window", "Clear Playlist", 0));
#endif // QT_NO_TOOLTIP
        btn_clear->setText(QString());
#ifndef QT_NO_TOOLTIP
        btn_repAll->setToolTip(QApplication::translate("Playlist_Window", "Repeat All", 0));
#endif // QT_NO_TOOLTIP
        btn_repAll->setText(QString());
        btn_repAll->setShortcut(QApplication::translate("Playlist_Window", "Enter", 0));
#ifndef QT_NO_TOOLTIP
        btn_shuffle->setToolTip(QApplication::translate("Playlist_Window", "Shuffle", 0));
#endif // QT_NO_TOOLTIP
        btn_shuffle->setText(QString());
        btn_shuffle->setShortcut(QApplication::translate("Playlist_Window", "Enter", 0));
#ifndef QT_NO_TOOLTIP
        btn_append->setToolTip(QApplication::translate("Playlist_Window", "Append when adding new tracks", 0));
#endif // QT_NO_TOOLTIP
        btn_append->setText(QString());
        btn_append->setShortcut(QApplication::translate("Playlist_Window", "Enter", 0));
#ifndef QT_NO_TOOLTIP
        btn_replace->setToolTip(QApplication::translate("Playlist_Window", "Replace with new tracks", 0));
#endif // QT_NO_TOOLTIP
        btn_replace->setText(QString());
        btn_replace->setShortcut(QApplication::translate("Playlist_Window", "Enter", 0));
#ifndef QT_NO_TOOLTIP
        btn_playAdded->setToolTip(QApplication::translate("Playlist_Window", "Directly play first added track", 0));
#endif // QT_NO_TOOLTIP
        btn_playAdded->setText(QString());
        btn_playAdded->setShortcut(QApplication::translate("Playlist_Window", "Enter", 0));
#ifndef QT_NO_TOOLTIP
        btn_numbers->setToolTip(QApplication::translate("Playlist_Window", "Show numbers in playlist", 0));
#endif // QT_NO_TOOLTIP
        btn_numbers->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class Playlist_Window: public Ui_Playlist_Window {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GUI_PLAYLIST_H
