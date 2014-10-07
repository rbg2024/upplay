/* Copyright (C) 2011  Lucio Carreras
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
#ifndef GUI_SIMPLEPLAYER_H
#define GUI_SIMPLEPLAYER_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QTranslator>

#include <QList>
#include <QCloseEvent>
#include <QKeySequence>
#include <QTranslator>
#include <QWidget>
#include <QAction>
#include <QDebug>
#include <QtNetwork/QNetworkAccessManager>

#include "ui_GUI_Player.h"
#include "GUI/player/GUI_TrayIcon.h"
#include "HelperStructs/Helper.h"

class GUI_Playlist;
class GUI_Library_windowed;
class GUI_InfoDialog;
class GUI_LibraryPath;
class GUI_Notifications;
class GUI_Startup_Dialog;
class CoverLookup;
class GUI_Alternate_Covers;

class GUI_Player : public QMainWindow {
    Q_OBJECT
public:
    explicit GUI_Player(QTranslator* translator, QWidget *parent = 0);
    ~GUI_Player();

public slots:

    void update_track (const MetaData & in, int pos=0, bool playing=true);
    void setCurrentPosition (quint32 pos_sec);

    void really_close(bool=false);

    void trayItemActivated (QSystemTrayIcon::ActivationReason reason);

    // Reflect externally triggered audio changes in ui
    void stopped();
    void playing();
    void paused();

    void setVolume(int vol);
    void setVolumeUi(int volume_percent);

    void setRendererName(const QString& nm);

signals:

    /* Player*/
    void play();
    void pause();
    void stop();
    void backward();
    void forward();
    void mute();
    void sig_volume_changed (int);
    void search(int pos_percent);


    /* File */
    void sig_choose_renderer();

    /* Preferences / View */
    void show_playlists();
    void show_small_playlist_items(bool);
    void sig_skin_changed(bool);

    /* Covers */
    void sig_want_cover(const MetaData &);
    void sig_fetch_alternate_covers(int);
    void sig_want_more_covers();
    void sig_fetch_all_covers();

    void sig_stream_selected(const QString&, const QString&);


private slots:
    void playClicked(bool b = true);
    void stopClicked(bool b = true);
    void backwardClicked(bool b = true);
    void forwardClicked(bool b = true);
    void coverClicked();
    void setProgressJump(int percent);
    void jump_forward();
    void jump_backward();

    void muteButtonPressed();
    void volumeChanged(int volume_percent);
    void volumeChangedByTick(int val);
    void volumeHigher();
    void volumeLower();

    /* File */
    void onChangeMediaRenderer();

    /* View */
    void showLibrary(bool, bool resize=true);
    void changeSkin(bool);
    void small_playlist_items_toggled(bool);
    void show_notification_toggled(bool);
    void show_fullscreen_toggled(bool);

    /* Preferences */

    void load_pl_on_startup_toggled(bool);
    void min2tray_toggled(bool);
    void only_one_instance_toggled(bool);

    void about(bool b=false);
    void help(bool b=false);

    void sl_no_cover_available();
    void sl_cover_fetch_done(QNetworkReply*);

public:
    void setPlaylist(GUI_Playlist* playlist);
    QWidget* getParentOfPlaylist();
    QWidget* getParentOfLibrary();
    void setLibraryWidget(QWidget* );
    void setPlaylistWidget(QWidget* );

    void ui_loaded();
    void setStyle(int);

protected:
    void closeEvent(QCloseEvent* e);
    void keyPressEvent(QKeyEvent* e);
    void resizeEvent(QResizeEvent* e);
    void moveEvent(QMoveEvent* e);

private:

    Ui::Upplay *ui;

    GUI_Playlist*           ui_playlist;
    GUI_Library_windowed*   ui_library;
    GUI_LibraryPath*        ui_libpath;
    GUI_InfoDialog*         ui_info_dialog;
    GUI_Notifications*      ui_notifications;
    GUI_Startup_Dialog*     ui_startup_dialog;
    CoverLookup*            m_cov_lookup;
    QNetworkAccessManager  *m_netmanager;
    GUI_Alternate_Covers*   m_alternate_covers;

    QString                 m_class_name;
    QString m_renderer_friendly_name;
    quint32                 m_completeLength_ms;
    bool                    m_playing;
    bool                    m_mute;
    GUI_TrayIcon *          m_trayIcon;

    QString                 m_skinSuffix;

    MetaData			m_metadata;
    MetaData			m_metadata_corrected;
    bool                m_metadata_available;
    bool                m_min2tray;

    int                 m_library_width;
    int                 m_library_stretch_factor;
    CSettingsStorage*   m_settings;
    QTranslator*        m_translator;


    void setupTrayActions ();

    void setVolume(int vol, bool dostore, bool doemit);
    void setupVolButton(int percent);
    void initGUI();
    void setupConnections();
    void total_time_changed(qint64);
    void fetch_cover(const QString&);
    QAction* createAction(QKeySequence key_sequence);
    QAction* createAction(QList<QKeySequence>& key_sequences);
};


#endif // GUI_SIMPLEPLAYER_H
