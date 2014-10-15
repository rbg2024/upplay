/* GUI_Player.cpp */

/* Copyright (C) 2012  Lucio Carreras
 *
 * This file is part of sayonara player
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QMessageBox>
#include <QFileDialog>
#include <QPalette>
#include <QUrl>
#include <QtNetwork/QNetworkRequest>

#include "ui_GUI_Player.h"
#include "GUI/player/GUI_Player.h"
#include "GUI/player/GUI_TrayIcon.h"
#include "GUI/playlist/GUI_Playlist.h"
#include "HelperStructs/CSettingsStorage.h"
#include "HelperStructs/Style.h"
#include "HelperStructs/globals.h"

GUI_Player* obj_ref = 0;

#ifdef Q_OS_UNIX
#include <signal.h>

void signal_handler(int sig)
{

    if (sig == SIGWINCH && obj_ref) {
        qDebug() << "show everything";
        obj_ref->setHidden(false);
        obj_ref->showNormal();
        obj_ref->activateWindow();
    }

    else if (sig == 15 && obj_ref) {
        qDebug() << "extern close event";
        obj_ref->really_close();
    } else {

        qDebug() << "signal " << sig << " received";
    }
}

#endif

GUI_Player::GUI_Player(QTranslator* translator, QWidget *parent) :
    QMainWindow(parent), ui(new Ui::Upplay), m_completeLength_ms(0)
{
    ui->setupUi(this);
    initGUI();
    m_translator = translator;
    m_settings = CSettingsStorage::getInstance();
    m_netmanager = new QNetworkAccessManager(this);
    connect(m_netmanager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(sl_cover_fetch_done(QNetworkReply*)));

    ui->albumCover->setIcon(QIcon(Helper::getIconPath() + "logo.png"));

    ui->lab_artist->hide();
    ui->lab_title->hide();
    ui->lab_rating->hide();
    ui->lab_album->hide();

    ui->lab_sayonara->setText(tr("Upplay Player"));
    ui->lab_version->setText(m_renderer_friendly_name.isEmpty() ?
                             m_settings->getVersion() :
                             m_renderer_friendly_name);
    ui->lab_writtenby->setText(tr("Based on Sayonara, by") + " Lucio Carreras");
    ui->lab_copyright->setText(tr("Copyright") + " 2011-2013");

    m_metadata_available = false;
    m_playing = false;
    m_mute = false;

    ui_playlist = 0;

    ui_startup_dialog = 0;//new GUI_Startup_Dialog(this);

    m_skinSuffix = "";
    m_class_name = "Player";

    m_cov_lookup = 0;//new CoverLookup();
    m_alternate_covers = 0;//new GUI_Alternate_Covers(this->centralWidget(), m_class_name);

    m_min2tray = m_settings->getMinimizeToTray();
    ui->action_min2tray->setChecked(m_min2tray);

    bool showSmallPlaylistItems = m_settings->getShowSmallPlaylist();
    ui->action_smallPlaylistItems->setChecked(showSmallPlaylistItems);

    QSizePolicy p = ui->library_widget->sizePolicy();
    m_library_stretch_factor = p.horizontalStretch();

    bool show_library = !m_settings->getNoShowLibrary();
    ui->action_viewLibrary->setChecked(show_library);
    this->showLibrary(show_library);

    bool is_fullscreen = m_settings->getPlayerFullscreen();
    if (!is_fullscreen) {
        QSize size = m_settings->getPlayerSize();
        QPoint pos = m_settings->getPlayerPos();

        QRect rect = this->geometry();
        rect.setX(pos.x());
        rect.setY(pos.y());
        rect.setWidth(size.width());
        rect.setHeight(size.height());
        this->setGeometry(rect);
    }

    m_library_width = 600;

    /* TRAY ACTIONS */
    this->setupTrayActions();

    /* SIGNALS AND SLOTS */
    this->setupConnections();
    ui_info_dialog = 0;

    changeSkin(m_settings->getPlayerStyle() == 1);
}


GUI_Player::~GUI_Player()
{
    qDebug() << "closing player...";
    delete ui;
}

void GUI_Player::setRendererName(const QString& nm)
{
    m_renderer_friendly_name = nm;
    ui->lab_version->setText(m_renderer_friendly_name.isEmpty() ?
                             m_settings->getVersion() :
                             m_renderer_friendly_name);
}

QAction* GUI_Player::createAction(QList<QKeySequence>& seq_list)
{
    QAction* action = new QAction(this);

    action->setShortcuts(seq_list);
    action->setShortcutContext(Qt::ApplicationShortcut);
    this->addAction(action);
    connect(this, SIGNAL(destroyed()), action, SLOT(deleteLater()));

    return action;
}

QAction* GUI_Player::createAction(QKeySequence seq)
{
    QList<QKeySequence> seq_list;
    seq_list << seq;
    return createAction(seq_list);
}


void GUI_Player::initGUI()
{
    ui->btn_mute->setIcon(QIcon(Helper::getIconPath() + "vol_1.png"));
    ui->btn_play->setIcon(QIcon(Helper::getIconPath() + "play.png"));
    ui->btn_stop->setIcon(QIcon(Helper::getIconPath() + "stop.png"));
    ui->btn_fw->setIcon(QIcon(Helper::getIconPath() + "fwd.png"));
    ui->btn_bw->setIcon(QIcon(Helper::getIconPath() + "bwd.png"));

    ui->action_viewLibrary->setText(tr("&Library"));

    ui->action_Fullscreen->setShortcut(QKeySequence("F11"));
    ui->action_Dark->setShortcut(QKeySequence("F10"));

}


// This is called from the playlist when new track info is known
// (possibly up from the audio player)
void GUI_Player::update_track(const MetaData& md, int pos_sec, bool playing)
{
    if (!m_metadata.compare(md)) {
        return;
    }
    m_metadata = md;

    m_completeLength_ms = md.length_ms;
    m_playing = playing;
    m_trayIcon->setPlaying(playing);

    if (pos_sec > 0) {
        setCurrentPosition(pos_sec);
    }

    ui->lab_sayonara->hide();
    ui->lab_title->show();

    ui->lab_version->hide();
    ui->lab_artist->show();

    ui->lab_writtenby->hide();
    ui->lab_album->show();

    ui->lab_copyright->hide();
    ui->lab_rating->show();

    // sometimes ignore the date
    if (md.year < 1000 || md.album.contains(QString::number(md.year))) {
        ui->lab_album->setText(Helper::get_album_w_disc(md));
    } else {
        ui->lab_album->setText(
            Helper::get_album_w_disc(md) + " (" + QString::number(md.year) + ")");
    }

    ui->lab_artist->setText(md.artist);
    ui->lab_title->setText(md.title);


    m_trayIcon->songChangedMessage(md);

    QString lengthString = Helper::cvtMsecs2TitleLengthString(md.length_ms, true);
    ui->maxTime->setText(lengthString);

    if (m_playing) {
        ui->btn_play->setIcon(QIcon(Helper::getIconPath() + "pause.png"));
    } else {
        ui->btn_play->setIcon(QIcon(Helper::getIconPath() + "play.png"));
    }

    QString tmp = QString("<font color=\"#FFAA00\" size=\"+10\">");
    if (md.bitrate < 96000) {
        tmp += "*";
    } else if (md.bitrate < 128000) {
        tmp += "**";
    } else if (md.bitrate < 160000) {
        tmp += "***";
    } else if (md.bitrate < 256000) {
        tmp += "****";
    } else {
        tmp += "*****";
    }
    tmp += "</font>";

    ui->lab_rating->setText(tmp);
    ui->lab_rating->setToolTip(
        QString("<font color=\"#000000\">") +
        QString::number(md.bitrate / 1000) +
        QString(" kBit/s") +
        QString("</font>"));

    this->setWindowTitle(QString("Upplay - ") + md.title);

    if (!md.albumArtURI.isEmpty()) {
        fetch_cover(md.albumArtURI);
    } else {
        ui->albumCover->setIcon(QIcon(Helper::getIconPath() + "logo.png"));
    }

    ui->btn_bw->setEnabled(true);
    ui->btn_fw->setEnabled(true);

    ui->btn_play->setVisible(true);
    ui->btn_play->setEnabled(true);

    m_trayIcon->set_enable_play(true);
    m_trayIcon->set_enable_fwd(true);
    m_trayIcon->set_enable_bwd(true);

    ui->songProgress->setEnabled(true);

    m_metadata_available = true;

    this->repaint();
}


void GUI_Player::fetch_cover(const QString& URI)
{
    if (!m_netmanager) {
        return;
    }
    m_netmanager->get(QNetworkRequest(QUrl(URI)));
}

void GUI_Player::setStyle(int style)
{

    bool dark = (style == 1);
    changeSkin(dark);
    ui->action_Dark->setChecked(dark);
}

void GUI_Player::changeSkin(bool dark)
{

    QString stylesheet = Style::get_style(dark);

    this->setStyleSheet(stylesheet);

    if (dark) {
        m_skinSuffix = QString("_dark");
    } else {
        m_skinSuffix = QString("");
    }

    m_settings->setPlayerStyle(dark ? 1 : 0);
    this->m_trayIcon->change_skin(stylesheet);

    setupVolButton(ui->volumeSlider->value());
    emit sig_skin_changed(dark);
}


/** TRAY ICON **/
void GUI_Player::setupTrayActions()
{
    m_trayIcon = new GUI_TrayIcon(this);

    connect(m_trayIcon, SIGNAL(sig_stop_clicked()), this, SLOT(stopClicked()));
    connect(m_trayIcon, SIGNAL(sig_bwd_clicked()), this, SLOT(backwardClicked()));
    connect(m_trayIcon, SIGNAL(sig_fwd_clicked()), this, SLOT(forwardClicked()));
    connect(m_trayIcon, SIGNAL(sig_mute_clicked()), this, SLOT(muteButtonPressed()));
    connect(m_trayIcon, SIGNAL(sig_close_clicked()), this, SLOT(really_close()));
    connect(m_trayIcon, SIGNAL(sig_play_clicked()), this, SLOT(playClicked()));
    connect(m_trayIcon, SIGNAL(sig_pause_clicked()), this, SLOT(playClicked()));
    connect(m_trayIcon, SIGNAL(sig_show_clicked()), this, SLOT(showNormal()));

    connect(m_trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this,       SLOT(trayItemActivated(QSystemTrayIcon::ActivationReason)));

    connect(m_trayIcon, SIGNAL(onVolumeChangedByWheel(int)),
            this,       SLOT(volumeChangedByTick(int)));


    m_trayIcon->setPlaying(false);

    m_trayIcon->show();
}

void GUI_Player::trayItemActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {

    case QSystemTrayIcon::Trigger:

        if (this->isMinimized() || isHidden()) {
            this->setHidden(false);
            this->showNormal();
            this->activateWindow();
        } else if (m_min2tray) {
            this->setHidden(true);
        } else {
            this->showMinimized();
        }

        break;
    case QSystemTrayIcon::MiddleClick:
        m_trayIcon->songChangedMessage(m_metadata);
        break;
    default:
        break;
    }
}

/** TRAY ICON END **/


/** LIBRARY AND PLAYLIST **/

QWidget* GUI_Player::getParentOfPlaylist()
{
    return ui->playlist_widget ? ui->playlist_widget->parentWidget() : 
        this->centralWidget();
}

QWidget* GUI_Player::getParentOfLibrary()
{
    return ui->library_widget ? ui->library_widget->parentWidget() : 
        this->centralWidget();
}

void GUI_Player::setPlaylistWidget(QWidget* w)
{
    delete ui->playlist_widget;
    ui->playlist_widget = w;
    ui->verticalLayout->addWidget(ui->playlist_widget);
}

void GUI_Player::setLibraryWidget(QWidget* w)
{
    if (ui->library_widget) {
        w->setSizePolicy(ui->library_widget->sizePolicy());
    } else {
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(2);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(w->sizePolicy().hasHeightForWidth());
        w->setSizePolicy(sizePolicy);
    }

    delete ui->library_widget;
    ui->library_widget = w;
    ui->splitter->addWidget(w);
}

void GUI_Player::setPlaylist(GUI_Playlist* playlist)
{
    ui_playlist = playlist;
    if (ui_playlist) {
        ui_playlist->show();
        ui_playlist->resize(ui->playlist_widget->size());
        QAction* action = createAction(QKeySequence(tr("Ctrl+P")));
        connect(action, SIGNAL(triggered()), ui_playlist, SLOT(setFocus()));
    }
}

void GUI_Player::stopped()
{
    qDebug() << "void GUI_Player::stopped()";
    m_metadata_available = false;
    stopClicked(false);
}
void GUI_Player::playing()
{
    qDebug() << "void GUI_Player::playing()";
    ui->btn_play->setIcon(QIcon(Helper::getIconPath() + "pause.png"));
    m_playing = true;
    m_trayIcon->setPlaying(m_playing);
}
void GUI_Player::paused()
{
    qDebug() << "void GUI_Player::paused()";
    ui->btn_play->setIcon(QIcon(Helper::getIconPath() + "play.png"));
    m_playing = false;
    m_trayIcon->setPlaying(m_playing);
}

/** LIBRARY AND PLAYLIST END **/


void GUI_Player::ui_loaded()
{
#ifdef Q_OS_UNIX
    obj_ref = this;
    signal(SIGWINCH, signal_handler);
#endif

    changeSkin(m_settings->getPlayerStyle() == 1);
    this->ui->action_Fullscreen->setChecked(m_settings->getPlayerFullscreen());
}

void GUI_Player::moveEvent(QMoveEvent *e)
{
    QMainWindow::moveEvent(e);

    QPoint p = this->pos();
    m_settings->setPlayerPos(p);
}

void GUI_Player::resizeEvent(QResizeEvent*e)
{
    QMainWindow::resizeEvent(e);
    m_settings->setPlayerSize(this->size());
    ui_playlist->resize(ui->playlist_widget->size());
}

void GUI_Player::keyPressEvent(QKeyEvent* e)
{

    e->accept();

    switch (e->key()) {

    case (Qt::Key_F10):
        ui->action_Dark->setChecked(!ui->action_Dark->isChecked());
        break;

    case (Qt::Key_F11):
        show_fullscreen_toggled(!this->isFullScreen());
        break;

    default:
        break;
    }
}

void GUI_Player::closeEvent(QCloseEvent* e)
{
    if (m_min2tray) {
        e->ignore();
        this->hide();
    }
}

void GUI_Player::really_close(bool)
{
    m_min2tray = false;
    this->close();
}
