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

#include "PlaylistAVT.h"

void PlaylistAVT::set_for_playing(int row)
{
    if (row < 0 || row >= int(_v_meta_data.size())) {
        _cur_play_idx = -1;
        _v_meta_data.setCurPlayTrack(-1);
        return;
    }

    if(!checkTrack(_v_meta_data[row]))
        return;

    emit sig_playing_track_changed(row);
    emit sig_play_now(_v_meta_data[row]);
    emit sig_track_metadata(_v_meta_data[row]);

    _cur_play_idx = row;
    _v_meta_data.setCurPlayTrack(row);
}

// Player switched tracks under us. Hopefully the uri matches a further track
void PlaylistAVT::psl_ext_track_change(const QString& uri)
{
    if (!valid_row(_cur_play_idx))
        return;

    for (unsigned int i = _cur_play_idx + 1; i < _v_meta_data.size(); i++) {
        if (!uri.compare(_v_meta_data[i].filepath)) {
            qDebug() << "PlaylistAVT::psl_ext_track_change: index now " << i;
            _cur_play_idx = i;
            _v_meta_data.setCurPlayTrack(i);
            emit sig_playing_track_changed(i);
            emit sig_track_metadata(_v_meta_data[i]);
            break;
        }
    }
}

void PlaylistAVT::send_next_playing_signal()
{
    // Only if there is a track behind the current one
    if (_cur_play_idx >= 0 && _cur_play_idx < int(_v_meta_data.size()) - 1)
        emit sig_next_track_to_play(_v_meta_data[_cur_play_idx + 1]);
}

void PlaylistAVT::psl_prepare_for_end_of_track()
{
    if (!valid_row(_cur_play_idx))
        return;
    send_next_playing_signal();
}

void PlaylistAVT::psl_new_transport_state(int tps)
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

void PlaylistAVT::psl_next_track()
{
    qDebug() << "PlaylistAVT::psl_next_track()";

    int track_num = -1;
    if(_v_meta_data.empty()) {
        qDebug() << "PlaylistAVT::psl_next_track(): empty playlist";
        goto out;
    }

    if (_playlist_mode.shuffle) {
        // shuffle mode
        track_num = rand() % _v_meta_data.size();
        if (track_num == _cur_play_idx) {
            track_num = (_cur_play_idx + 1) % _v_meta_data.size();
        }
    } else {
        if (_cur_play_idx >= int(_v_meta_data.size()) -1) {
            // last track
            qDebug() << "PlaylistAVT::psl_next_track(): was last, stop or loop";
            if(_playlist_mode.repAll) {
                track_num = 0;
            }
        } else {
            track_num = _cur_play_idx + 1;
            qDebug() << "PlaylistAVT::psl_next_track(): new tnum " << track_num;
        }
    }

out:
    if (track_num >= 0) {
        // valid next track
        if(checkTrack(_v_meta_data[track_num])){
            set_for_playing(track_num);
        } else {
            remove_row(track_num);
            psl_next_track();
        }
    } else {
        set_for_playing(-1);
        emit sig_stop();
        return;
    }
}

// GUI -->
void PlaylistAVT::psl_clear_playlist()
{
    _v_meta_data.clear();
    _cur_play_idx = -1;
    emit sig_playlist_updated(_v_meta_data, _cur_play_idx, 0);
}

void PlaylistAVT::psl_play()
{
    _pause = false;

    if (_v_meta_data.empty()) {
        return;
    }

    if (_cur_play_idx < 0) {
        if (checkTrack(_v_meta_data[0])) {
            set_for_playing(0);
        }
    } else {
        emit sig_resume_play();
    }
}

void PlaylistAVT::psl_pause()
{
    _pause = true;
    emit sig_pause();
}

void PlaylistAVT::psl_stop()
{
    set_for_playing(-1);
    emit sig_stop();
    emit sig_playlist_updated(_v_meta_data, _cur_play_idx, 0);
}

// fwd was pressed -> next track
void PlaylistAVT::psl_forward()
{
    psl_next_track();
}

// GUI -->
void PlaylistAVT::psl_backward()
{
    if (_cur_play_idx <= 0) {
        return;
    }

    int track_num = _cur_play_idx - 1;

    if (checkTrack(_v_meta_data[track_num])) {
        set_for_playing(track_num);
    }
}

// GUI -->
void PlaylistAVT::psl_change_track(int new_row)
{
    if (!valid_row(new_row)) {
        return;
    }

    if (checkTrack(_v_meta_data[new_row])) {
        set_for_playing(new_row);
    } else {
        set_for_playing(-1);
        remove_row(new_row);
        emit sig_stop();
        emit sig_playlist_updated(_v_meta_data, _cur_play_idx, 0);
    }
}

// insert tracks after idx which may be -1
void PlaylistAVT::psl_insert_tracks(const MetaDataList& nmeta, int row)
{
    // Change to more conventional "insert before"
    ++row;

    qDebug() << "PlaylistAVT::psl_insert_tracks: cur size" << 
        _v_meta_data.size() << " before row " << row;
    if (_v_meta_data.empty()) {
        if (row != 0) {
            return;
        }
    } else if (row < 0 || row > int(_v_meta_data.size())) {
        qDebug() << " bad row";
        return;
    }
    
    _v_meta_data.insert(_v_meta_data.begin() + row, nmeta.begin(), nmeta.end());

    // Find the playing track (could be part of nmeta if this is a d&d
    // cut/paste) and adjust _cur_play_idx
    _cur_play_idx = -1;
    for (auto it = _v_meta_data.begin(); it != _v_meta_data.end(); it++) {
        if (it->pl_playing && _cur_play_idx == -1) {
            _cur_play_idx = it - _v_meta_data.begin();
        } else {
            it->pl_playing = false;
        }
    }

    send_next_playing_signal();

    emit sig_playlist_updated(_v_meta_data, _cur_play_idx, 0);
}

void PlaylistAVT::psl_append_tracks(const MetaDataList& v_md)
{
    psl_insert_tracks(v_md, _v_meta_data.size() - 1);
}

void PlaylistAVT::psl_remove_rows(const QList<int>& rows, bool select_next_row)
{
    if (rows.empty()) {
        return;
    }

    MetaDataList v_tmp_md;

    int n_tracks = (int) _v_meta_data.size();
    int n_tracks_before_cur_idx = 0;

    if (rows.contains(_cur_play_idx)) {
        _cur_play_idx = -1;
    }

    int first_row = rows[0];
    for (int i = 0; i < n_tracks; i++) {

        if (rows.contains(i)) {
            if (i < _cur_play_idx) {
                n_tracks_before_cur_idx++;
            }
            continue;
        }

        MetaData md = _v_meta_data[i];

        md.pl_dragged = false;
        md.pl_selected = false;
        md.pl_playing = false;

        v_tmp_md.push_back(md);
    }

    _cur_play_idx -= n_tracks_before_cur_idx;

    if (_cur_play_idx < 0 || _cur_play_idx >= (int) v_tmp_md.size()) {
        _cur_play_idx = -1;
    } else {
        v_tmp_md[_cur_play_idx].pl_playing = true;
    }

    _v_meta_data = v_tmp_md;

    if (select_next_row) {
        _v_meta_data[first_row].pl_selected = true;
    }

    emit sig_playlist_updated(_v_meta_data, _cur_play_idx, 0);
}


// GUI -->
void PlaylistAVT::psl_playlist_mode_changed(const Playlist_Mode& playlist_mode)
{
    _settings->setPlaylistMode(playlist_mode);
    _playlist_mode = playlist_mode;
    _playlist_mode.print();
}
