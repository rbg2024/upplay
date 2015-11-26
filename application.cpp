/* Copyright (C) 2013  Lucio Carreras
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

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

#include <QApplication>
#include <QMap>
#include <QSharedMemory>
#include <QMessageBox>

#include "application.h"
#include "GUI/renderchoose/renderchoose.h"

#include "playlist/PlaylistAVT.h"
#include "playlist/PlaylistOH.h"
#include "GUI/prefs/prefs.h"

#include "HelperStructs/Helper.h"
#include "HelperStructs/CSettingsStorage.h"
#include "HelperStructs/Style.h"
#include "HelperStructs/globals.h"

#include "libupnpp/upnpplib.hxx"
#include "libupnpp/control/mediarenderer.hxx"
#include "libupnpp/control/renderingcontrol.hxx"
#include "libupnpp/control/discovery.hxx"

using namespace UPnPClient;

#ifndef deleteZ
#define deleteZ(X) {delete X; X = 0;}
#endif

#define CONNECT(a,b,c,d) m_app->connect(a, SIGNAL(b), c, SLOT(d), \
                                        Qt::UniqueConnection)

UPnPDeviceDirectory *superdir;

static MRDH getRenderer(const string& name, bool isfriendlyname)
{
    if (superdir == 0) {
        superdir = UPnPDeviceDirectory::getTheDir();
        if (superdir == 0) {
            cerr << "Can't create UPnP discovery object" << endl;
            exit(1);
        }
    }

    UPnPDeviceDesc ddesc;
    if (isfriendlyname) {
        if (superdir->getDevByFName(name, ddesc)) {
            return MRDH(new MediaRenderer(ddesc));
        }
        cerr << "getDevByFname failed" << endl;
    } else {
        if (superdir->getDevByUDN(name, ddesc)) {
            return MRDH(new MediaRenderer(ddesc));
        }
        cerr << "getDevByFname failed" << endl;
    }
    return MRDH();
}

bool Application::is_initialized()
{
    return m_initialized;
}

bool Application::setupRenderer(const string& uid)
{
    deleteZ(m_playlist);
    deleteZ(m_rdco);
    deleteZ(m_avto);
    deleteZ(m_ohtmo);
    deleteZ(m_ohvlo);

    // Create media renderer object. We don't use it directly but it
    // gives handles to the services. Note that the lib will return
    // anything implementing either renderingcontrol or ohproduct
    MRDH rdr = getRenderer(uid, false);
    if (!rdr) {
        cerr << "Renderer " << uid << " not found" << endl;
        return false;
    }

    if (rdr->rdc()) {
        qDebug() << "Application::setupRenderer: using Rendering Control";
        m_rdco = new RenderingControlQO(rdr->rdc());
    } else {
        if (!rdr->ohvl()) {
            cerr << "Device implements neither RenderingControl nor OHVolume" <<
                endl;
            return false;
        }
        qDebug() << "Application::setupRenderer: using OHVolume";
        m_ohvlo =  new OHVolumeQO(rdr->ohvl());
    }

    bool needavt = true;
    OHPLH ohpl = rdr->ohpl();
    if (ohpl) {
        qDebug() << "Application::setupRenderer: using OHPlaylist";
        OHTMH ohtm = rdr->ohtm();
        if (ohtm) {
            qDebug() << "Application::setupRenderer: OHTm ok, no need for avt";
            m_ohtmo = new OHTimeQO(ohtm);
            // no need for AVT then
            needavt = false;
        }
        m_playlist = new PlaylistOH(new OHPlayer(ohpl));
    }

    if (needavt) {
        if (!rdr->avt()) {
            cerr << "Renderer: AVTransport missing but we need it" << endl;
            return false;
        }
        m_avto = new AVTPlayer(rdr->avt());
    }

    if (!ohpl) {
        qDebug() << "Application::setupRenderer: using AVT playlist";
        m_playlist = new PlaylistAVT(m_avto, rdr->desc()->UDN);
    }
    
    m_cdb->setPlaylist(m_playlist);

    QString fn = QString::fromUtf8(rdr->desc()->friendlyName.c_str());
    if (m_player) {
        m_player->setRendererName(fn);
    }

    renderer_connections();

    return true;
}

void Application::chooseRenderer()
{
    vector<UPnPDeviceDesc> devices;
    if (!MediaRenderer::getDeviceDescs(devices) || devices.empty()) {
        QMessageBox::warning(0, "Upplay",
                             tr("No Media Renderers found."));
        return;
    }
    RenderChooseDLG dlg(m_player);
    for (vector<UPnPDeviceDesc>::iterator it = devices.begin(); 
         it != devices.end(); it++) {
        dlg.rndsLW->addItem(QString::fromUtf8(it->friendlyName.c_str()));
    }
    if (!dlg.exec()) {
        return;
    }

    int row = dlg.rndsLW->currentRow();
    if (row < 0 || row >= int(devices.size())) {
        cerr << "Internal error: bad row after renderer choose dlg" << endl;
        return;
    }
    MetaDataList curmeta;
    if (m_playlist) {
        m_playlist->get_metadata(curmeta);
    }

    QString friendlyname = QString::fromUtf8(devices[row].friendlyName.c_str());
    if (!setupRenderer(devices[row].UDN)) {
        QMessageBox::warning(0, "Upplay", tr("Can't connect to ") +
                             friendlyname);
        return;
    }
    m_settings->setPlayerUID(QString::fromUtf8(devices[row].UDN.c_str()));

    if (m_playlist && !dlg.keepRB->isChecked()) {
        if (dlg.replRB->isChecked()) {
            m_playlist->psl_clear_playlist();
        }
        m_playlist->psl_add_tracks(curmeta);
    }
}

Application::Application(QApplication* qapp, int,
                         QTranslator* translator, QObject *parent)
    : QObject(parent), m_player(0), m_playlist(0), m_cdb(0), m_rdco(0),
      m_avto(0), m_ohtmo(0), m_ohvlo(0), m_ui_playlist(0),
      m_settings(0), m_app(qapp), m_initialized(false)
{
    m_settings = CSettingsStorage::getInstance();

    QString version = getVersion();
    m_settings->setVersion(version);

    m_player = new GUI_Player(translator);

    m_ui_playlist = new GUI_Playlist(m_player->getParentOfPlaylist(), 0);
    m_player->setPlaylistWidget(m_ui_playlist);

    m_cdb = new DirBrowser(m_player->getParentOfLibrary(), 0);
    m_player->setLibraryWidget(m_cdb);

    init_connections();
    string uid = qs2utf8s(m_settings->getPlayerUID());
    if (uid.empty()) {
        QTimer::singleShot(0, this, SLOT(chooseRenderer()));
    } else {
        if (!setupRenderer(uid)) {
            cerr << "Can't connect to previous media renderer" << endl;
            QTimer::singleShot(0, this, SLOT(chooseRenderer()));
        }
    }

    m_player->setWindowTitle("Upplay " + version);
    m_player->setWindowIcon(QIcon(Helper::getIconPath() + "logo.png"));
    m_player->setPlaylist(m_ui_playlist);
    m_player->setStyle(m_settings->getPlayerStyle());
    m_player->show();

    m_ui_playlist->resize(m_player->getParentOfPlaylist()->size());

    m_player->ui_loaded();

    m_initialized = true;
}

Application::~Application()
{
    delete m_player;
}

void Application::reconnectOrChoose()
{
    string uid = qs2utf8s(m_settings->getPlayerUID());
    if (uid.empty() || !setupRenderer(uid)) {
        if (QMessageBox::warning(0, "Upplay",
                             tr("Connection to current rendererer lost. "
                                "Choose again ?"),
                             QMessageBox::Cancel | QMessageBox::Ok, 
                                 QMessageBox::Ok) == QMessageBox::Ok) {
            chooseRenderer();
        }
    }
}

void Application::renderer_connections()
{
    // Use either ohtime or avt for time updates
    if (m_ohtmo) {
        CONNECT(m_ohtmo, secsInSongChanged(quint32),
                m_player, setCurrentPosition(quint32));
        CONNECT(m_ohtmo, secsInSongChanged(quint32),
                m_playlist, psl_secs_in_song(quint32));
    } else if (m_avto) {
        CONNECT(m_avto, secsInSongChanged(quint32),
                m_player, setCurrentPosition(quint32));
        CONNECT(m_avto, secsInSongChanged(quint32),
                m_playlist, psl_secs_in_song(quint32));
    }
    if (m_ohvlo) {
        CONNECT(m_player, sig_volume_changed(int), m_ohvlo, setVolume(int));
        CONNECT(m_player, sig_mute(bool), m_ohvlo, setMute(bool));
        CONNECT(m_ohvlo, volumeChanged(int), m_player, setVolumeUi(int));
        CONNECT(m_ohvlo, muteChanged(bool), m_player, setMuteUi(bool));
        // Set up the initial volume from the renderer value
        m_player->setVolumeUi(m_ohvlo->volume());
    } else if (m_rdco) {
        CONNECT(m_player, sig_volume_changed(int), m_rdco, setVolume(int));
        CONNECT(m_player, sig_mute(bool), m_rdco, setMute(bool));
        CONNECT(m_rdco, volumeChanged(int), m_player, setVolumeUi(int));
        CONNECT(m_rdco, muteChanged(bool), m_player, setMuteUi(bool));
        // Set up the initial volume from the renderer value
        m_player->setVolumeUi(m_rdco->volume());
    }

    CONNECT(m_player, play(), m_playlist, psl_play());
    CONNECT(m_player, pause(), m_playlist, psl_pause());
    CONNECT(m_player, stop(), m_playlist, psl_stop());
    CONNECT(m_player, forward(), m_playlist, psl_forward());
    CONNECT(m_player, backward(), m_playlist, psl_backward());
    CONNECT(m_player, sig_load_playlist(), m_playlist, psl_load_playlist());
    CONNECT(m_player, sig_sortprefs(), m_cdb, onSortprefs());
    CONNECT(m_player, sig_save_playlist(), m_playlist, psl_save_playlist());
    CONNECT(m_player, sig_seek(int), m_playlist, psl_seek(int));

    CONNECT(m_playlist, connectionLost(), this, reconnectOrChoose());
    CONNECT(m_playlist, playlistModeChanged(Playlist_Mode),
            m_ui_playlist, setPlayerMode(Playlist_Mode));
    CONNECT(m_playlist, sig_track_metadata(const MetaData&),
            m_player, update_track(const MetaData&));
    CONNECT(m_playlist, sig_stopped(),  m_player, stopped());
    CONNECT(m_playlist, sig_paused(),  m_player, paused());
    CONNECT(m_playlist, sig_playing(),  m_player, playing());
    CONNECT(m_playlist, sig_playing_track_changed(int),
            m_ui_playlist, track_changed(int));
    CONNECT(m_playlist, sig_playlist_updated(MetaDataList&, int, int),
            m_ui_playlist, fillPlaylist(MetaDataList&, int, int));
    CONNECT(m_ui_playlist, selection_min_row(int),
            m_playlist, psl_selection_min_row(int));
    CONNECT(m_ui_playlist, playlist_mode_changed(const Playlist_Mode&),
            m_playlist, psl_change_mode(const Playlist_Mode&));
    CONNECT(m_ui_playlist, dropped_tracks(const MetaDataList&, int),
            m_playlist, psl_insert_tracks(const MetaDataList&, int));
    CONNECT(m_ui_playlist, sig_rows_removed(const QList<int>&, bool),
            m_playlist, psl_remove_rows(const QList<int>&, bool));
    CONNECT(m_ui_playlist, sig_sort_tno(), m_playlist, psl_sort_by_tno());
    CONNECT(m_ui_playlist, row_activated(int),
            m_playlist, psl_change_track(int));
    CONNECT(m_ui_playlist, clear_playlist(), m_playlist, psl_clear_playlist());
    CONNECT(m_cdb, sig_next_group_html(QString),
            m_ui_playlist, psl_next_group_html(QString));
}

// Connections which make sense without a renderer.
void Application::init_connections()
{
    CONNECT(m_player, show_small_playlist_items(bool),
            m_ui_playlist, psl_show_small_playlist_items(bool));
    CONNECT(m_player, sig_choose_renderer(), this, chooseRenderer());
    CONNECT(m_player, sig_skin_changed(bool), m_cdb, setStyleSheet(bool));
    CONNECT(m_player, showSearchPanel(bool), m_cdb, showSearchPanel(bool));
    static UPPrefs g_prefs(m_player);
    CONNECT(m_player, sig_preferences(), &g_prefs, onShowPrefs());
    CONNECT(&g_prefs, sig_prefsChanged(), m_cdb, onSortprefs());
}


QString Application::getVersion()
{
    return UPPLAY_VERSION;
}

QMainWindow* Application::getMainWindow()
{
    return this->m_player;
}
