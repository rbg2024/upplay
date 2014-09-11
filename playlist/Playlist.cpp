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
 *
 */

#include <ctime>

using namespace std;
#include <QFile>
#include <QList>
#include <QObject>
#include <QDate>
#include <QTime>
#include <QDebug>
#include <QDir>

#include <libupnpp/control/avtransport.hxx>

#include "Playlist.h"
#include "HelperStructs/MetaData.h"
#include "HelperStructs/Helper.h"
#include "HelperStructs/CSettingsStorage.h"
#include "HelperStructs/PlaylistMode.h"


Playlist::Playlist(QObject * parent) 
    : QObject (parent)
{
    _settings = CSettingsStorage::getInstance();
    _playlist_mode = _settings->getPlaylistMode();
    _v_meta_data.clear();
    _cur_play_idx = -1;
    _is_playing = false;
    _pause = false;
}

Playlist::~Playlist() 
{
}

// create a playlist, where metadata is already available
void Playlist::psl_create_playlist(MetaDataList& v_meta_data){

    bool instant_play = ((_v_meta_data.size() == 0) && (!_is_playing));

    if(!_playlist_mode.append){
        _v_meta_data.clear();
        _cur_play_idx = -1;
    }

    // no tracks in new playlist
    if(v_meta_data.size() == 0) 
    {
        emit sig_playlist_updated(_v_meta_data, _cur_play_idx, 0);
        return;
    }

    foreach(MetaData md, v_meta_data){
        _v_meta_data.push_back(md);
    }

    emit sig_playlist_updated(_v_meta_data, _cur_play_idx, 0);

    if(instant_play)
        send_cur_playing_signal(0);

}

void Playlist::send_cur_playing_signal(int i) 
{
    qDebug() << "Playlist::send_cur_playing_signal: " << i;
    if (i < 0 && i >= int(_v_meta_data.size()))
        return;

    _is_playing = true;

    if(!checkTrack(_v_meta_data[i])) 
        return;

    emit sig_playing_track_changed(i);
    emit sig_play_now(_v_meta_data[i]);
    emit sig_track_metadata(_v_meta_data[i]);
    _cur_play_idx = i;
}

// Player switched tracks under us. We get the URI
void Playlist::psl_ext_track_change(const QString& uri)
{
    if (_cur_play_idx < 0 || _cur_play_idx >= int(_v_meta_data.size()) - 1)
        return;
    for (unsigned int i = _cur_play_idx + 1; i < _v_meta_data.size(); i++) {
        if (!uri.compare(_v_meta_data[i].filepath)) {
            qDebug() << "Playlist::psl_ext_track_change: index now " << i;
            _cur_play_idx = i;
            emit sig_playing_track_changed(i);
            emit sig_track_metadata(_v_meta_data[i]);
            break;
        }
    }
}

void Playlist::send_next_playing_signal(int i)
{
    if (i >= 0 && i < int(_v_meta_data.size()))
        emit sig_next_track_to_play(_v_meta_data[i]);
}

void Playlist::psl_prepare_for_end_of_track()
{
    if (_cur_play_idx < 0 || _cur_play_idx >= int(_v_meta_data.size()) - 1)
        return;
    send_next_playing_signal(_cur_play_idx+1);
}

void Playlist::psl_new_transport_state(int tps)
{
    string s;
    switch (tps) {
    case UPnPClient::AVTransport::Stopped: s = "Stopped"; break;
    case UPnPClient::AVTransport::Playing: s = "Playing"; break;
    case UPnPClient::AVTransport::Transitioning: s = "Transitioning"; break;
    case UPnPClient::AVTransport::PausedPlayback: s = "PausedPlayback"; break;
    case UPnPClient::AVTransport::PausedRecording: s = "PausedRecording"; break;
    case UPnPClient::AVTransport::Recording: s = "Recording"; break;
    case UPnPClient::AVTransport::NoMediaPresent: s = "NoMediaPresent"; break;
    case UPnPClient::AVTransport::Unknown: 
    default:
        s = "Unknown"; break;
    }
    qDebug() << "psl_new_transport_state: " << s.c_str();
}

void Playlist::psl_next_track()
{
    qDebug() << "Playlist::psl_next_track()";

    int track_num = -1;
    if(_v_meta_data.size() == 0){
        qDebug() << "Playlist::psl_next_track(): empty playlist";
        _cur_play_idx = -1;
        _is_playing = false;
        emit sig_stopped();
        return;
    }

    // shuffle mode
    if(_playlist_mode.shuffle) {
        track_num = rand() % _v_meta_data.size();
        if (track_num == _cur_play_idx) {
            track_num = (_cur_play_idx + 1) % _v_meta_data.size();
        }
    } else {
        // last track?
        if(_cur_play_idx >= (int) _v_meta_data.size() -1) {
            qDebug() << "Playlist::psl_next_track(): was last, stop";
            track_num = -1;

            if(_playlist_mode.repAll) {
                track_num = 0;
            } else {
                _is_playing = false;
                _cur_play_idx = -1;
                emit sig_stopped();
                return;
            }
        } else {
            track_num = _cur_play_idx + 1;
            qDebug() << "Playlist::psl_next_track(): new tnum " << track_num;
        }
    }

    // valid next track
    if(track_num >= 0) {
        _v_meta_data.setCurPlayTrack(track_num);
        MetaData md = _v_meta_data[track_num];

        if(checkTrack(md)){
            send_cur_playing_signal(track_num);
        } else {
            remove_row(track_num);
            psl_next_track();
        }
    }    
}

uint Playlist::get_num_tracks()
{
    return _v_meta_data.size();
}

