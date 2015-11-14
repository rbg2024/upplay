/********************************************************************************
** Form generated from reading UI file 'GUI_Player.ui'
**
** Created by: Qt User Interface Compiler version 5.5.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GUI_PLAYER_H
#define UI_GUI_PLAYER_H

#include <QtCore/QLocale>
#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "GUI/player/DirectSlider.h"

QT_BEGIN_NAMESPACE

class Ui_Upplay
{
public:
    QAction *action_Close;
    QAction *action_Dark;
    QAction *action_help;
    QAction *action_about;
    QAction *action_viewLibrary;
    QAction *action_min2tray;
    QAction *action_sortprefs;
    QAction *action_Fullscreen;
    QAction *action_smallPlaylistItems;
    QAction *actionChange_Media_Renderer;
    QAction *action_viewSearchPanel;
    QAction *actionSave_Playlist;
    QAction *actionLoad_Playlist;
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout_2;
    QSplitter *splitter;
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout;
    QGridLayout *gridLayout;
    QPushButton *btn_stop;
    QLabel *lab_title;
    QPushButton *albumCover;
    DirectSlider *songProgress;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *btn_bw;
    QPushButton *btn_play;
    QPushButton *btn_fw;
    DirectSlider *volumeSlider;
    QFrame *line;
    QHBoxLayout *horizontalLayout_4;
    QLabel *curTime;
    QSpacerItem *horizontalSpacer;
    QLabel *maxTime;
    QPushButton *btn_mute;
    QLabel *lab_artist;
    QLabel *lab_album;
    QHBoxLayout *horizontalLayout_8;
    QLabel *lab_version;
    QLabel *lab_rating;
    QWidget *playlist_widget;
    QWidget *library_widget;
    QMenuBar *menubar;
    QMenu *menuFle;
    QMenu *menuView;
    QMenu *menuPreferences;
    QMenu *menuAbout;

    void setupUi(QMainWindow *Upplay)
    {
        if (Upplay->objectName().isEmpty())
            Upplay->setObjectName(QStringLiteral("Upplay"));
        Upplay->resize(550, 383);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(Upplay->sizePolicy().hasHeightForWidth());
        Upplay->setSizePolicy(sizePolicy);
        Upplay->setFocusPolicy(Qt::StrongFocus);
        QIcon icon;
        icon.addFile(QStringLiteral("../../.designer/backup/icons/play.png"), QSize(), QIcon::Normal, QIcon::Off);
        Upplay->setWindowIcon(icon);
        Upplay->setToolButtonStyle(Qt::ToolButtonIconOnly);
        Upplay->setDocumentMode(false);
        Upplay->setUnifiedTitleAndToolBarOnMac(true);
        action_Close = new QAction(Upplay);
        action_Close->setObjectName(QStringLiteral("action_Close"));
        action_Dark = new QAction(Upplay);
        action_Dark->setObjectName(QStringLiteral("action_Dark"));
        action_Dark->setCheckable(true);
        action_help = new QAction(Upplay);
        action_help->setObjectName(QStringLiteral("action_help"));
        action_about = new QAction(Upplay);
        action_about->setObjectName(QStringLiteral("action_about"));
        action_viewLibrary = new QAction(Upplay);
        action_viewLibrary->setObjectName(QStringLiteral("action_viewLibrary"));
        action_viewLibrary->setCheckable(true);
        action_viewLibrary->setChecked(true);
        action_viewLibrary->setShortcutContext(Qt::ApplicationShortcut);
        action_min2tray = new QAction(Upplay);
        action_min2tray->setObjectName(QStringLiteral("action_min2tray"));
        action_min2tray->setCheckable(true);
        action_sortprefs = new QAction(Upplay);
        action_sortprefs->setObjectName(QStringLiteral("action_sortprefs"));
        action_sortprefs->setCheckable(false);
        action_Fullscreen = new QAction(Upplay);
        action_Fullscreen->setObjectName(QStringLiteral("action_Fullscreen"));
        action_Fullscreen->setCheckable(true);
        action_smallPlaylistItems = new QAction(Upplay);
        action_smallPlaylistItems->setObjectName(QStringLiteral("action_smallPlaylistItems"));
        action_smallPlaylistItems->setCheckable(true);
        actionChange_Media_Renderer = new QAction(Upplay);
        actionChange_Media_Renderer->setObjectName(QStringLiteral("actionChange_Media_Renderer"));
        action_viewSearchPanel = new QAction(Upplay);
        action_viewSearchPanel->setObjectName(QStringLiteral("action_viewSearchPanel"));
        action_viewSearchPanel->setCheckable(true);
        actionSave_Playlist = new QAction(Upplay);
        actionSave_Playlist->setObjectName(QStringLiteral("actionSave_Playlist"));
        actionLoad_Playlist = new QAction(Upplay);
        actionLoad_Playlist->setObjectName(QStringLiteral("actionLoad_Playlist"));
        centralwidget = new QWidget(Upplay);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        verticalLayout_2 = new QVBoxLayout(centralwidget);
        verticalLayout_2->setSpacing(0);
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        splitter = new QSplitter(centralwidget);
        splitter->setObjectName(QStringLiteral("splitter"));
        splitter->setOrientation(Qt::Horizontal);
        layoutWidget = new QWidget(splitter);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        verticalLayout = new QVBoxLayout(layoutWidget);
        verticalLayout->setSpacing(2);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setHorizontalSpacing(4);
        gridLayout->setVerticalSpacing(0);
        btn_stop = new QPushButton(layoutWidget);
        btn_stop->setObjectName(QStringLiteral("btn_stop"));
        btn_stop->setMinimumSize(QSize(28, 28));
        btn_stop->setMaximumSize(QSize(28, 28));
        btn_stop->setFocusPolicy(Qt::NoFocus);
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/icons/stop.png"), QSize(), QIcon::Normal, QIcon::Off);
        btn_stop->setIcon(icon1);
        btn_stop->setIconSize(QSize(22, 22));
        btn_stop->setFlat(true);

        gridLayout->addWidget(btn_stop, 7, 3, 2, 1);

        lab_title = new QLabel(layoutWidget);
        lab_title->setObjectName(QStringLiteral("lab_title"));
        lab_title->setMinimumSize(QSize(0, 50));
        lab_title->setMaximumSize(QSize(16777215, 50));
        QFont font;
        font.setPointSize(10);
        font.setBold(true);
        font.setWeight(75);
        lab_title->setFont(font);
        lab_title->setStyleSheet(QStringLiteral(""));
        lab_title->setLocale(QLocale(QLocale::German, QLocale::Germany));
        lab_title->setInputMethodHints(Qt::ImhNone);
        lab_title->setText(QStringLiteral(""));
        lab_title->setTextFormat(Qt::PlainText);
        lab_title->setScaledContents(true);
        lab_title->setAlignment(Qt::AlignCenter);
        lab_title->setWordWrap(true);
        lab_title->setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::TextSelectableByMouse);

        gridLayout->addWidget(lab_title, 0, 1, 1, 2);

        albumCover = new QPushButton(layoutWidget);
        albumCover->setObjectName(QStringLiteral("albumCover"));
        albumCover->setMinimumSize(QSize(80, 80));
        albumCover->setMaximumSize(QSize(95, 95));
        albumCover->setFocusPolicy(Qt::NoFocus);
        albumCover->setIconSize(QSize(80, 80));
        albumCover->setFlat(true);

        gridLayout->addWidget(albumCover, 0, 0, 7, 1);

        songProgress = new DirectSlider(layoutWidget);
        songProgress->setObjectName(QStringLiteral("songProgress"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(1);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(songProgress->sizePolicy().hasHeightForWidth());
        songProgress->setSizePolicy(sizePolicy1);
        songProgress->setMinimumSize(QSize(100, 20));
        songProgress->setMaximumSize(QSize(16777215, 20));
        songProgress->setSizeIncrement(QSize(3, 0));
        songProgress->setFocusPolicy(Qt::NoFocus);
        songProgress->setOrientation(Qt::Horizontal);
        songProgress->setInvertedAppearance(false);
        songProgress->setInvertedControls(false);
        songProgress->setTickInterval(20);

        gridLayout->addWidget(songProgress, 7, 1, 1, 2);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(0);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setSizeConstraint(QLayout::SetMaximumSize);
        horizontalLayout_2->setContentsMargins(-1, -1, -1, 0);
        btn_bw = new QPushButton(layoutWidget);
        btn_bw->setObjectName(QStringLiteral("btn_bw"));
        btn_bw->setEnabled(true);
        QSizePolicy sizePolicy2(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(btn_bw->sizePolicy().hasHeightForWidth());
        btn_bw->setSizePolicy(sizePolicy2);
        btn_bw->setMinimumSize(QSize(28, 28));
        btn_bw->setMaximumSize(QSize(28, 28));
        btn_bw->setFocusPolicy(Qt::NoFocus);
        QIcon icon2;
        icon2.addFile(QStringLiteral(":/icons/bwd.png"), QSize(), QIcon::Normal, QIcon::Off);
        btn_bw->setIcon(icon2);
        btn_bw->setIconSize(QSize(20, 20));
        btn_bw->setFlat(true);

        horizontalLayout_2->addWidget(btn_bw);

        btn_play = new QPushButton(layoutWidget);
        btn_play->setObjectName(QStringLiteral("btn_play"));
        sizePolicy2.setHeightForWidth(btn_play->sizePolicy().hasHeightForWidth());
        btn_play->setSizePolicy(sizePolicy2);
        btn_play->setMinimumSize(QSize(32, 34));
        btn_play->setMaximumSize(QSize(32, 34));
        btn_play->setFocusPolicy(Qt::NoFocus);
        QIcon icon3;
        icon3.addFile(QStringLiteral(":/icons/play.png"), QSize(), QIcon::Normal, QIcon::Off);
        btn_play->setIcon(icon3);
        btn_play->setIconSize(QSize(28, 28));
        btn_play->setAutoDefault(false);
        btn_play->setFlat(true);

        horizontalLayout_2->addWidget(btn_play);

        btn_fw = new QPushButton(layoutWidget);
        btn_fw->setObjectName(QStringLiteral("btn_fw"));
        btn_fw->setEnabled(true);
        sizePolicy2.setHeightForWidth(btn_fw->sizePolicy().hasHeightForWidth());
        btn_fw->setSizePolicy(sizePolicy2);
        btn_fw->setMinimumSize(QSize(28, 28));
        btn_fw->setMaximumSize(QSize(28, 28));
        btn_fw->setFocusPolicy(Qt::NoFocus);
        QIcon icon4;
        icon4.addFile(QStringLiteral(":/icons/fwd.png"), QSize(), QIcon::Normal, QIcon::Off);
        btn_fw->setIcon(icon4);
        btn_fw->setIconSize(QSize(20, 20));
        btn_fw->setFlat(true);

        horizontalLayout_2->addWidget(btn_fw);


        gridLayout->addLayout(horizontalLayout_2, 7, 0, 2, 1);

        volumeSlider = new DirectSlider(layoutWidget);
        volumeSlider->setObjectName(QStringLiteral("volumeSlider"));
        volumeSlider->setFocusPolicy(Qt::NoFocus);
        volumeSlider->setMaximum(100);
        volumeSlider->setValue(50);
        volumeSlider->setTracking(false);
        volumeSlider->setInvertedAppearance(false);
        volumeSlider->setInvertedControls(false);
        volumeSlider->setTickInterval(25);

        gridLayout->addWidget(volumeSlider, 1, 3, 6, 1);

        line = new QFrame(layoutWidget);
        line->setObjectName(QStringLiteral("line"));
        line->setEnabled(true);
        QSizePolicy sizePolicy3(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
        sizePolicy3.setHorizontalStretch(3);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(line->sizePolicy().hasHeightForWidth());
        line->setSizePolicy(sizePolicy3);
        line->setMinimumSize(QSize(320, 3));
        line->setStyleSheet(QStringLiteral(""));
        line->setFrameShadow(QFrame::Sunken);
        line->setLineWidth(1);
        line->setMidLineWidth(0);
        line->setFrameShape(QFrame::HLine);

        gridLayout->addWidget(line, 9, 0, 1, 4);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(2);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        curTime = new QLabel(layoutWidget);
        curTime->setObjectName(QStringLiteral("curTime"));
        QFont font1;
        font1.setPointSize(7);
        curTime->setFont(font1);
        curTime->setStyleSheet(QStringLiteral(""));
        curTime->setLineWidth(0);
        curTime->setText(QStringLiteral(""));

        horizontalLayout_4->addWidget(curTime);

        horizontalSpacer = new QSpacerItem(40, 10, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer);

        maxTime = new QLabel(layoutWidget);
        maxTime->setObjectName(QStringLiteral("maxTime"));
        maxTime->setFont(font1);
        maxTime->setLineWidth(0);
        maxTime->setText(QStringLiteral(""));
        maxTime->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_4->addWidget(maxTime);


        gridLayout->addLayout(horizontalLayout_4, 8, 1, 1, 2);

        btn_mute = new QPushButton(layoutWidget);
        btn_mute->setObjectName(QStringLiteral("btn_mute"));
        btn_mute->setMaximumSize(QSize(28, 30));
        btn_mute->setFocusPolicy(Qt::NoFocus);
        QIcon icon5;
        icon5.addFile(QStringLiteral(":/icons/vol_mute.png"), QSize(), QIcon::Normal, QIcon::Off);
        btn_mute->setIcon(icon5);
        btn_mute->setIconSize(QSize(24, 24));
        btn_mute->setFlat(true);

        gridLayout->addWidget(btn_mute, 0, 3, 1, 1);

        lab_artist = new QLabel(layoutWidget);
        lab_artist->setObjectName(QStringLiteral("lab_artist"));
        lab_artist->setMinimumSize(QSize(0, 30));
        lab_artist->setMaximumSize(QSize(16777215, 30));
        QFont font2;
        font2.setPointSize(6);
        font2.setStyleStrategy(QFont::PreferAntialias);
        lab_artist->setFont(font2);
        lab_artist->setStyleSheet(QStringLiteral(""));
        lab_artist->setFrameShape(QFrame::NoFrame);
        lab_artist->setFrameShadow(QFrame::Plain);
        lab_artist->setText(QStringLiteral(""));
        lab_artist->setTextFormat(Qt::AutoText);
        lab_artist->setAlignment(Qt::AlignCenter);
        lab_artist->setWordWrap(true);
        lab_artist->setOpenExternalLinks(false);

        gridLayout->addWidget(lab_artist, 3, 1, 2, 2);

        lab_album = new QLabel(layoutWidget);
        lab_album->setObjectName(QStringLiteral("lab_album"));
        lab_album->setMinimumSize(QSize(0, 30));
        lab_album->setMaximumSize(QSize(16777215, 30));
        lab_album->setFont(font2);
        lab_album->setStyleSheet(QStringLiteral(""));
        lab_album->setText(QStringLiteral(""));
        lab_album->setAlignment(Qt::AlignCenter);
        lab_album->setWordWrap(true);

        gridLayout->addWidget(lab_album, 5, 1, 2, 2);

        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setObjectName(QStringLiteral("horizontalLayout_8"));
        lab_version = new QLabel(layoutWidget);
        lab_version->setObjectName(QStringLiteral("lab_version"));
        lab_version->setMinimumSize(QSize(0, 20));
        lab_version->setMaximumSize(QSize(16777215, 20));
        QFont font3;
        font3.setPointSize(8);
        font3.setStyleStrategy(QFont::PreferAntialias);
        lab_version->setFont(font3);
        lab_version->setAlignment(Qt::AlignCenter);

        horizontalLayout_8->addWidget(lab_version);

        lab_rating = new QLabel(layoutWidget);
        lab_rating->setObjectName(QStringLiteral("lab_rating"));
        lab_rating->setMinimumSize(QSize(0, 20));
        lab_rating->setMaximumSize(QSize(16777215, 20));
        QFont font4;
        font4.setPointSize(8);
        font4.setBold(true);
        font4.setWeight(75);
        font4.setStyleStrategy(QFont::PreferAntialias);
        lab_rating->setFont(font4);
        lab_rating->setAutoFillBackground(false);
        lab_rating->setStyleSheet(QStringLiteral(""));
        lab_rating->setText(QStringLiteral(""));
        lab_rating->setAlignment(Qt::AlignCenter);

        horizontalLayout_8->addWidget(lab_rating);


        gridLayout->addLayout(horizontalLayout_8, 1, 1, 2, 2);


        verticalLayout->addLayout(gridLayout);

        playlist_widget = new QWidget(layoutWidget);
        playlist_widget->setObjectName(QStringLiteral("playlist_widget"));
        QSizePolicy sizePolicy4(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(1);
        sizePolicy4.setHeightForWidth(playlist_widget->sizePolicy().hasHeightForWidth());
        playlist_widget->setSizePolicy(sizePolicy4);
        playlist_widget->setMinimumSize(QSize(200, 0));

        verticalLayout->addWidget(playlist_widget);

        verticalLayout->setStretch(1, 1);
        splitter->addWidget(layoutWidget);
        library_widget = new QWidget(splitter);
        library_widget->setObjectName(QStringLiteral("library_widget"));
        QSizePolicy sizePolicy5(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy5.setHorizontalStretch(2);
        sizePolicy5.setVerticalStretch(0);
        sizePolicy5.setHeightForWidth(library_widget->sizePolicy().hasHeightForWidth());
        library_widget->setSizePolicy(sizePolicy5);
        library_widget->setMinimumSize(QSize(200, 0));
        library_widget->setFocusPolicy(Qt::StrongFocus);
        library_widget->setStyleSheet(QStringLiteral(""));
        splitter->addWidget(library_widget);

        verticalLayout_2->addWidget(splitter);

        Upplay->setCentralWidget(centralwidget);
        menubar = new QMenuBar(Upplay);
        menubar->setObjectName(QStringLiteral("menubar"));
        menubar->setGeometry(QRect(0, 0, 550, 25));
        menubar->setDefaultUp(false);
        menubar->setNativeMenuBar(false);
        menuFle = new QMenu(menubar);
        menuFle->setObjectName(QStringLiteral("menuFle"));
        menuView = new QMenu(menubar);
        menuView->setObjectName(QStringLiteral("menuView"));
        menuView->setStyleSheet(QStringLiteral(""));
        menuPreferences = new QMenu(menubar);
        menuPreferences->setObjectName(QStringLiteral("menuPreferences"));
        menuAbout = new QMenu(menubar);
        menuAbout->setObjectName(QStringLiteral("menuAbout"));
        Upplay->setMenuBar(menubar);

        menubar->addAction(menuFle->menuAction());
        menubar->addAction(menuView->menuAction());
        menubar->addAction(menuPreferences->menuAction());
        menubar->addAction(menuAbout->menuAction());
        menuFle->addAction(actionChange_Media_Renderer);
        menuFle->addSeparator();
        menuFle->addAction(actionSave_Playlist);
        menuFle->addAction(actionLoad_Playlist);
        menuFle->addSeparator();
        menuFle->addAction(action_Close);
        menuView->addAction(action_viewLibrary);
        menuView->addAction(action_viewSearchPanel);
        menuView->addAction(action_smallPlaylistItems);
        menuView->addSeparator();
        menuView->addAction(action_Dark);
        menuView->addAction(action_Fullscreen);
        menuPreferences->addAction(action_min2tray);
        menuPreferences->addAction(action_sortprefs);
        menuAbout->addAction(action_help);
        menuAbout->addAction(action_about);

        retranslateUi(Upplay);

        btn_play->setDefault(false);


        QMetaObject::connectSlotsByName(Upplay);
    } // setupUi

    void retranslateUi(QMainWindow *Upplay)
    {
        Upplay->setWindowTitle(QApplication::translate("Upplay", "Upplay Player", 0));
        action_Close->setText(QApplication::translate("Upplay", "Close", 0));
        action_Close->setShortcut(QApplication::translate("Upplay", "Ctrl+Q", 0));
        action_Dark->setText(QApplication::translate("Upplay", "Dark", 0));
        action_help->setText(QApplication::translate("Upplay", "Help", 0));
        action_about->setText(QApplication::translate("Upplay", "About", 0));
        action_viewLibrary->setText(QApplication::translate("Upplay", "&Library", 0));
        action_min2tray->setText(QApplication::translate("Upplay", "Close to tray", 0));
        action_sortprefs->setText(QApplication::translate("Upplay", "Sort preferences", 0));
        action_Fullscreen->setText(QApplication::translate("Upplay", "Fullscreen ", 0));
        action_smallPlaylistItems->setText(QApplication::translate("Upplay", "Small Playlist Items", 0));
        actionChange_Media_Renderer->setText(QApplication::translate("Upplay", "Change Media Renderer", 0));
        action_viewSearchPanel->setText(QApplication::translate("Upplay", "Search panel", 0));
        actionSave_Playlist->setText(QApplication::translate("Upplay", "Save Playlist", 0));
        actionLoad_Playlist->setText(QApplication::translate("Upplay", "Load Playlist", 0));
        btn_stop->setText(QString());
        albumCover->setText(QString());
        btn_bw->setText(QString());
        btn_play->setText(QString());
        btn_play->setShortcut(QString());
        btn_fw->setText(QString());
        btn_mute->setText(QString());
        lab_version->setText(QString());
        menuFle->setTitle(QApplication::translate("Upplay", "File", 0));
        menuView->setTitle(QApplication::translate("Upplay", "View", 0));
        menuPreferences->setTitle(QApplication::translate("Upplay", "Preferences", 0));
        menuAbout->setTitle(QApplication::translate("Upplay", "Help", 0));
    } // retranslateUi

};

namespace Ui {
    class Upplay: public Ui_Upplay {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GUI_PLAYER_H
