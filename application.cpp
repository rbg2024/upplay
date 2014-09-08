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

#include "application.h"

#include <QApplication>
#include "GUI/player/GUI_Player.h"
#include "GUI/playlist/GUI_Playlist.h"

#include "playlist/Playlist.h"

#include "HelperStructs/Helper.h"
#include "HelperStructs/CSettingsStorage.h"
#include "HelperStructs/Style.h"
#include "HelperStructs/globals.h"

#include <QMap>
#include <QSharedMemory>

#include <iostream>
#include <fstream>
#include <string>

#include "libupnpp/upnpplib.hxx"
#include "libupnpp/control/mediarenderer.hxx"
#include "libupnpp/control/renderingcontrol.hxx"
#include "libupnpp/control/discovery.hxx"
using namespace UPnPClient;

using namespace std;

UPnPDeviceDirectory *superdir;

MRDH getRenderer(const string& friendlyName)
{
    if (superdir == 0) {
        superdir = UPnPDeviceDirectory::getTheDir();
    }

    UPnPDeviceDesc ddesc;
    if (superdir->getDevByFName(friendlyName, ddesc)) {
        return MRDH(new MediaRenderer(ddesc));
    }
    cerr << "getDevByFname failed" << endl;
    return MRDH();
}

bool Application::is_initialized()
{
    return _initialized;
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
    _setting_thread  = new SettingsThread(player);
    playlist = new Playlist();

    ui_playlist = new GUI_Playlist(player->getParentOfPlaylist(), 0);

    cdb = new CDBrowser(player->getParentOfLibrary());

    const string friendlyName("UpMpd-bureau");
//    const string friendlyName("BubbleUPnP (NookColor)");

    MRDH rdr = getRenderer(friendlyName);
    if (!rdr) {
        cerr << "Renderer " << friendlyName << " not found" << endl;
        return;
    }

    AVTH avt = rdr->avt();
    if (!avt) {
        cerr << "Device " << friendlyName << 
            " has no AVTransport service" << endl;
        return;
    }

    RDCH rdc = rdr->rdc();
    if (!rdc) {
        cerr << "Device " << friendlyName << 
            " has no RenderingControl service" << endl;
        return;
    }

    rdco = new RenderingControlQO(rdc);
    avto = new AVTPlayer(avt);


    QString dir;

#ifdef Q_OS_UNIX
    dir = "/usr/lib/sayonara";
#else
    dir = app->applicationDirPath();
#endif

    // The actual audio player. MediaRenderer here.
    // listen = engine_plugin_loader->get_cur_engine();

    init_connections();

    qDebug() << "setting up player";
    player->setWindowTitle("Sayonara " + version);
    player->setWindowIcon(QIcon(Helper::getIconPath() + "logo.png"));

    player->setPlaylist(ui_playlist);

    player->setStyle( set->getPlayerStyle() );
    player->show();
    cdb->show();

    ui_playlist->resize(player->getParentOfPlaylist()->size());

    int vol = set->getVolume();
    player->setVolume(vol);

    player->ui_loaded();

    _initialized = true;

    _setting_thread->start();
}

Application::~Application()
{
    _setting_thread->stop();

    delete ui_playlist;
    delete playlist;
    delete player;
    
}

void Application::init_connections()
{
    CONNECT(player, pause(), avto, pause());
    // the search (actually seek) param is in percent
    CONNECT(player, search(int), avto, seekPC(int));
    CONNECT(avto, secsInSongChanged(quint32), 
            player, setCurrentPosition(quint32));
    CONNECT(player, sig_volume_changed(int), rdco, setVolume(int));
    CONNECT(rdco, volumeChanged(int), player, setVolumeUi(int));
    CONNECT(player, fileSelected(QStringList &), playlist, psl_createPlaylist(QStringList&));

    CONNECT(player, play(), playlist, psl_play());
    CONNECT(player, pause(), playlist, psl_pause());
    CONNECT(player, stop(), playlist, psl_stop());

    CONNECT(player, forward(), playlist, psl_forward());
    CONNECT(player, backward(), playlist, psl_backward());

    CONNECT(player, show_small_playlist_items(bool), ui_playlist, psl_show_small_playlist_items(bool));

    CONNECT(playlist, sig_selected_file_changed_md(const MetaData&, int, bool),
            player, update_track(const MetaData&, int, bool));
    CONNECT(playlist, sig_selected_file_changed_md(const MetaData&, int, bool),
            avto, changeTrack(const MetaData&, int, bool));

    CONNECT(playlist, sig_no_track_to_play(),  avto, stop());
    CONNECT(playlist, sig_no_track_to_play(),  player, stopped());
    CONNECT(playlist, sig_goon_playing(), avto, play());
    CONNECT(playlist, sig_selected_file_changed(int), ui_playlist, track_changed(int));
    CONNECT(playlist, sig_playlist_created(MetaDataList&, int, int), ui_playlist, fillPlaylist(MetaDataList&, int, int));

    CONNECT(ui_playlist, selected_row_changed(int),  playlist, psl_change_track(int));
    CONNECT(ui_playlist, clear_playlist(), playlist, psl_clear_playlist());
    CONNECT(ui_playlist, playlist_mode_changed(const Playlist_Mode&), playlist, psl_playlist_mode_changed(const Playlist_Mode&));
    CONNECT(ui_playlist, dropped_tracks(const MetaDataList&, int), playlist, psl_insert_tracks(const MetaDataList&, int));
    CONNECT(ui_playlist, sig_rows_removed(const QList<int>&, bool), playlist, psl_remove_rows(const QList<int>&, bool));

//    CONNECT(cdb, sig_tracks_for_playlist_available(MetaDataList&),
//            playlist, psl_createPlaylist(MetaDataList&));
//    CONNECT(library, sig_append_tracks_to_playlist(MetaDataList&),
//            playlist, psl_append_tracks(MetaDataList&));
    CONNECT(cdb, sig_tracks_for_playlist_available(MetaDataList&),
            playlist, psl_append_tracks(MetaDataList&));
}


QString Application::getVersion()
{

    ifstream istr;
    QString version_file = Helper::getSharePath() + "/VERSION";

    istr.open(version_file.toUtf8()  );
    if(!istr || !istr.is_open() ) return "0.3.1";

    QMap<QString, int> map;

    while(istr.good()){
        string type;
        int version;
        istr >> type >> version;
        if(type.size() > 0)
            qDebug() << type.c_str() << ": " << version;

        map[QString(type.c_str())] = version;
    }

    istr.close();

    QString version_str = QString::number(map["MAJOR"]) + "." +
        QString::number(map["MINOR"]) + "." +
        QString::number(map["SUBMINOR"]) + " r" + QString::number(map["BUILD"]);
    return version_str;
}

QMainWindow* Application::getMainWindow()
{
    return this->player;
}
