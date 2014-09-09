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

#include <QString>

#include "playlist/Playlist.h"
#include "HelperStructs/globals.h"
#include "HelperStructs/MetaData.h"
#include "HelperStructs/CSettingsStorage.h"


// GUI -->
void Playlist::psl_clear_playlist()
{
    _v_meta_data.clear();
    _cur_play_idx = -1;
    emit sig_playlist_created(_v_meta_data, _cur_play_idx, 0);
}


// play a track
void Playlist::psl_play()
{

    bool pause_old = _pause;
    _pause = false;

    if (pause_old) {
        emit sig_goon_playing();
        return;
    }

    if(_v_meta_data.size() == 0) 
        return;

    // state was stop until now
    if (_cur_play_idx < 0) {

        int track_num = 0;
        MetaData md = _v_meta_data[track_num];

        if(checkTrack(md)){
            send_cur_playing_signal(track_num);
        }
    } else {
        emit sig_goon_playing();
    }
}

void Playlist::psl_pause()
{
    _pause = true;
}

void Playlist::psl_stop()
{
    _cur_play_idx = -1;
    _is_playing = false;
    emit sig_no_track_to_play();
    emit sig_playlist_created(_v_meta_data, _cur_play_idx, 0);
}

// fwd was pressed -> next track
void Playlist::psl_forward()
{
    psl_next_track();
}

// GUI -->
void Playlist::psl_backward()
{
    if (_cur_play_idx <= 0) 
        return;

    int track_num = _cur_play_idx - 1;
    MetaData md = _v_meta_data[track_num];

    if (checkTrack(md)) {
        _v_meta_data.setCurPlayTrack(track_num);
        send_cur_playing_signal(track_num);
    }
}

// GUI -->
void Playlist::psl_change_track(int new_row)
{
    if(uint(new_row) >= _v_meta_data.size()) 
        return;

    for (uint i = 0; i < _v_meta_data.size(); i++) {
        _v_meta_data[i].pl_playing = (new_row == (int)i);
        _v_meta_data[i].pl_selected = false;
    }

    MetaData md = _v_meta_data[new_row];
    if (checkTrack(md)) {
       send_cur_playing_signal(new_row);
    } else {
        _cur_play_idx = -1;
        _v_meta_data.setCurPlayTrack(_cur_play_idx);
        remove_row(new_row);
        _is_playing = false;
        emit sig_no_track_to_play();
    }
}

// insert tracks (also drag & drop)
void Playlist::psl_insert_tracks(const MetaDataList& v_metadata, int row){

    //bool instant_play = ((_v_meta_data.size() == 0) && (!_is_playing));

    // possibly the current playing index has to be updated
    if(row < _cur_play_idx && _cur_play_idx != -1)
        _cur_play_idx += v_metadata.size();

    // insert new tracks
    for (uint i = 0; i < v_metadata.size(); i++) {

        MetaData md = v_metadata[i];
        _v_meta_data.insert_mid(md, i + row + 1);
        if (md.pl_playing) 
            _cur_play_idx = (i + row + 1);
    }


    psl_save_playlist_to_storage();
    emit sig_playlist_created(_v_meta_data, _cur_play_idx, 0);
}

void Playlist::psl_append_tracks(MetaDataList& v_md)
{
    if (v_md.size() == 0)
        return;

    int initsize = int(_v_meta_data.size());

    foreach (MetaData md, v_md)
        _v_meta_data.push_back(md);

    if (_cur_play_idx == initsize - 1) {
        // We were playing the last track. Set new next track for gapless, 
        send_next_playing_signal(_cur_play_idx + 1);
    }
    emit sig_playlist_created(_v_meta_data, _cur_play_idx, 0);
}



// remove one row
void Playlist::remove_row(int row)
{
    QList<int> remove_list;
    remove_list << row;
    psl_remove_rows(remove_list);
}

void Playlist::psl_remove_rows(const QList<int> & rows, bool select_next_row)
{
    if(rows.size() == 0) 
        return;

    MetaDataList v_tmp_md;

    int n_tracks = (int) _v_meta_data.size();
    int n_tracks_before_cur_idx = 0;

    if(rows.contains(_cur_play_idx)) 
        _cur_play_idx = -1;

    int first_row = rows[0];
    for (int i = 0; i < n_tracks; i++) {

        if (rows.contains(i)) {
            if (i < _cur_play_idx)
                n_tracks_before_cur_idx++;
            continue;
        }

        MetaData md = _v_meta_data[i];

        md.pl_dragged = false;
        md.pl_selected = false;
        md.pl_playing = false;

        v_tmp_md.push_back(md);
    }

    _cur_play_idx -= n_tracks_before_cur_idx;

    if(_cur_play_idx < 0 || _cur_play_idx >= (int) v_tmp_md.size() )
        _cur_play_idx = -1;
    else
        v_tmp_md[_cur_play_idx].pl_playing = true;

    _v_meta_data = v_tmp_md;

    if(select_next_row)
        _v_meta_data[first_row].pl_selected = true;

    psl_save_playlist_to_storage();

    emit sig_playlist_created(_v_meta_data, _cur_play_idx, 0);
}


// GUI -->
void Playlist::psl_playlist_mode_changed(const Playlist_Mode& playlist_mode)
{
    _settings->setPlaylistMode(playlist_mode);
    _playlist_mode = playlist_mode;
    _playlist_mode.print();
}


