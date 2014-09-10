/* application.cpp */

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
#include <functional>

using namespace std;

#include <QApplication>
#include <QMap>
#include <QSharedMemory>
#include <QMessageBox>

#include "application.h"
#include "GUI/player/GUI_Player.h"
#include "GUI/playlist/GUI_Playlist.h"
#include "GUI/renderchoose/renderchoose.h"
#include "playlist/Playlist.h"
#include "HelperStructs/Helper.h"
#include "HelperStructs/CSettingsStorage.h"
#include "HelperStructs/Style.h"
#include "HelperStructs/globals.h"

#include "libupnpp/upnpplib.hxx"
#include "libupnpp/control/mediarenderer.hxx"
#include "libupnpp/control/renderingcontrol.hxx"
#include "libupnpp/control/discovery.hxx"
using namespace UPnPClient;

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
    return _initialized;
}

bool Application::setupRenderer(const string& uid)
{
    delete rdco;
    delete avto;
    rdco = 0;
    avto = 0;

    MRDH rdr = getRenderer(uid, false);
    if (!rdr) {
        cerr << "Renderer " << uid << " not found" << endl;
        return false;
    }

    AVTH avt = rdr->avt();
    if (!avt) {
        cerr << "Device " << uid << 
            " has no AVTransport service" << endl;
        return false;
    }

    RDCH rdc = rdr->rdc();
    if (!rdc) {
        cerr << "Device " << uid << 
            " has no RenderingControl service" << endl;
        return false;
    }

    rdco = new RenderingControlQO(rdc);
    avto = new AVTPlayer(avt);
    QString fn = QString::fromUtf8(rdr->m_desc.friendlyName.c_str());
    if (player) {
        player->setRendererName(fn);
    }
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
    RenderChooseDLG dlg(player);
    for (auto it = devices.begin(); it != devices.end(); it++) {
        dlg.rndsLW->addItem(QString::fromUtf8(it->friendlyName.c_str()));
    }
    if (!dlg.exec())
        return;

    int row = dlg.rndsLW->currentRow();
    if (row < 0 || row >= int(devices.size())) {
        cerr << "Internal error: bad row after renderer choose dlg" << endl;
        return;
    }

    QString friendlyname = QString::fromUtf8(devices[row].friendlyName.c_str());
    if (!setupRenderer(devices[row].UDN)) {
        QMessageBox::warning(0, "Upplay", tr("Can't connect to ") + 
                             friendlyname);
        return;
    }
    set->setPlayerUID(QString::fromUtf8(devices[row].UDN.c_str()));
    renderer_connections();
}

Application::Application(QApplication* qapp, int, 
                         QTranslator* translator, QObject *parent)
  : QObject(parent)
{
    app = qapp;
    set = CSettingsStorage::getInstance();

    QString version = getVersion();
    set->setVersion(version);

    player = new GUI_Player(translator);
    playlist = new Playlist();

    ui_playlist = new GUI_Playlist(player->getParentOfPlaylist(), 0);

    cdb = new CDBrowser(player->getParentOfLibrary());
    
    rdco = 0;
    avto = 0;

    string uid = qs2utf8s(set->getPlayerUID());
    if (uid.empty()) {
        QTimer::singleShot(0, this, SLOT(chooseRenderer()));
    } else {
        if (!setupRenderer(uid)) {
            cerr << "Can't connect to media renderer" << endl;
            exit(1);
        }
    }

    init_connections();

    player->setWindowTitle("Upplay " + version);
    player->setWindowIcon(QIcon(Helper::getIconPath() + "logo.png"));
    player->setPlaylist(ui_playlist);
    player->setStyle( set->getPlayerStyle() );
    player->show();

    ui_playlist->resize(player->getParentOfPlaylist()->size());

    player->ui_loaded();

    _initialized = true;
}

Application::~Application()
{
}

void Application::renderer_connections()
{
    CONNECT(player, pause(), avto, pause());
    // the search (actually seek) param is in percent
    CONNECT(player, search(int), avto, seekPC(int));
    CONNECT(avto, secsInSongChanged(quint32), 
            player, setCurrentPosition(quint32));

    CONNECT(player, sig_volume_changed(int), rdco, setVolume(int));
    CONNECT(rdco, volumeChanged(int), player, setVolumeUi(int));
    CONNECT(playlist, sig_play_now(const MetaData&, int, bool),
            avto, changeTrack(const MetaData&, int, bool));
    CONNECT(avto, endOfTrackIsNear(), playlist, psl_prepare_for_the_end());
    CONNECT(avto, tpStateChanged(int), playlist, psl_new_transport_state(int));
    CONNECT(avto, stoppedAtEOT(), playlist, psl_next_track());
    CONNECT(avto, newTrackPlaying(const QString&), 
            playlist, psl_ext_track_change(const QString&));
    CONNECT(playlist, sig_next_track_to_play(const MetaData&),
            avto, infoNextTrack(const MetaData&));
    CONNECT(playlist, sig_no_track_to_play(),  avto, stop());
    CONNECT(playlist, sig_goon_playing(), avto, play());
}

void Application::init_connections()
{
    if (avto && rdco)
        renderer_connections();

    CONNECT(player, play(), playlist, psl_play());
    CONNECT(player, pause(), playlist, psl_pause());
    CONNECT(player, stop(), playlist, psl_stop());
    CONNECT(player, forward(), playlist, psl_forward());
    CONNECT(player, backward(), playlist, psl_backward());

    CONNECT(player, show_small_playlist_items(bool), 
            ui_playlist, psl_show_small_playlist_items(bool));
    CONNECT(player, sig_choose_renderer(), this, chooseRenderer());

    CONNECT(playlist, sig_track_metadata(const MetaData&, int, bool),
            player, update_track(const MetaData&, int, bool));
    CONNECT(playlist, sig_no_track_to_play(),  player, stopped());
    CONNECT(playlist, sig_selected_file_changed(int), 
            ui_playlist, track_changed(int));
    CONNECT(playlist, sig_playlist_created(MetaDataList&, int, int), 
            ui_playlist, fillPlaylist(MetaDataList&, int, int));

    CONNECT(ui_playlist, selected_row_changed(int),  
            playlist, psl_change_track(int));
    CONNECT(ui_playlist, clear_playlist(), playlist, psl_clear_playlist());
    CONNECT(ui_playlist, playlist_mode_changed(const Playlist_Mode&), 
            playlist, psl_playlist_mode_changed(const Playlist_Mode&));
    CONNECT(ui_playlist, dropped_tracks(const MetaDataList&, int), 
            playlist, psl_insert_tracks(const MetaDataList&, int));
    CONNECT(ui_playlist, sig_rows_removed(const QList<int>&, bool), 
            playlist, psl_remove_rows(const QList<int>&, bool));

    CONNECT(cdb, sig_tracks_for_playlist_available(MetaDataList&),
            playlist, psl_append_tracks(MetaDataList&));
}


QString Application::getVersion()
{
    return UPPLAY_VERSION;
}

QMainWindow* Application::getMainWindow()
{
    return this->player;
}
