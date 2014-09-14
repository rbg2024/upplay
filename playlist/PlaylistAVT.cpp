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
    qDebug() << "PlaylistAVT::set_for_playing " << row;
    if (row < 0 || row >= int(m_meta.size())) {
        m_play_idx = -1;
        m_meta.setCurPlayTrack(-1);
        return;
    }

    m_play_idx = row;
    m_meta.setCurPlayTrack(row);
    
    emit sig_playing_track_changed(row);
    emit sig_play_now(m_meta[row]);
    emit sig_track_metadata(m_meta[row], 0, true);
}

// Player switched tracks under us. Hopefully the uri matches a further track
void PlaylistAVT::psl_ext_track_change(const QString& uri)
{
    if (!valid_row(m_play_idx))
        return;

    for (unsigned int i = m_play_idx + 1; i < m_meta.size(); i++) {
        if (!uri.compare(m_meta[i].filepath)) {
            qDebug() << "PlaylistAVT::psl_ext_track_change: index now " << i;
            m_play_idx = i;
            m_meta.setCurPlayTrack(i);
            emit sig_playing_track_changed(i);
            emit sig_track_metadata(m_meta[i], -1, true);
            break;
        }
    }
}

void PlaylistAVT::send_next_playing_signal()
{
    // Do not do this in shuffle mode: makes no sense + as we never go
    // through the stop state, shuffle does not work
    if (_playlist_mode.shuffle) 
        return;
    // Only if there is a track behind the current one
    if (m_play_idx >= 0 && m_play_idx < int(m_meta.size()) - 1)
        emit sig_next_track_to_play(m_meta[m_play_idx + 1]);
}

void PlaylistAVT::psl_prepare_for_end_of_track()
{
    if (!valid_row(m_play_idx))
        return;
    send_next_playing_signal();
}

void PlaylistAVT::psl_next_track()
{
    qDebug() << "PlaylistAVT::psl_next_track()";

    int track_num = -1;
    if(m_meta.empty()) {
        qDebug() << "PlaylistAVT::psl_next_track(): empty playlist";
        goto out;
    }

    if (_playlist_mode.shuffle) {
        // shuffle mode
        track_num = rand() % m_meta.size();
        if (track_num == m_play_idx) {
            track_num = (m_play_idx + 1) % m_meta.size();
        }
    } else {
        if (m_play_idx >= int(m_meta.size()) -1) {
            // last track
            qDebug() << "PlaylistAVT::psl_next_track(): was last, stop or loop";
            if(_playlist_mode.repAll) {
                track_num = 0;
            }
        } else {
            track_num = m_play_idx + 1;
            qDebug() << "PlaylistAVT::psl_next_track(): new tnum " << track_num;
        }
    }

out:
    if (track_num >= 0) {
        // valid next track
        if(checkTrack(m_meta[track_num])){
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
    m_meta.clear();
    m_play_idx = -1;
    emit sig_playlist_updated(m_meta, m_play_idx, 0);
}

void PlaylistAVT::psl_play()
{
    _pause = false;

    if (m_meta.empty()) {
        return;
    }

    if (m_play_idx < 0) {
        if (valid_row(m_selection_min_row)) {
            set_for_playing(m_selection_min_row);
        } else {
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
    emit sig_playlist_updated(m_meta, m_play_idx, 0);
}

// fwd was pressed -> next track
void PlaylistAVT::psl_forward()
{
    psl_next_track();
}

// GUI -->
void PlaylistAVT::psl_backward()
{
    if (m_play_idx <= 0) {
        return;
    }

    int track_num = m_play_idx - 1;

    if (checkTrack(m_meta[track_num])) {
        set_for_playing(track_num);
    }
}

// GUI -->
void PlaylistAVT::psl_change_track(int new_row)
{
    if (!valid_row(new_row)) {
        return;
    }

    if (checkTrack(m_meta[new_row])) {
        set_for_playing(new_row);
    } else {
        set_for_playing(-1);
        remove_row(new_row);
        emit sig_stop();
        emit sig_playlist_updated(m_meta, m_play_idx, 0);
    }
}

// insert tracks after idx which may be -1
void PlaylistAVT::psl_insert_tracks(const MetaDataList& nmeta, int row)
{
    // Change to more conventional "insert before"
    ++row;

    qDebug() << "PlaylistAVT::psl_insert_tracks: cur size" << 
        m_meta.size() << ". " << nmeta.size() << " rows before row " << row;
    if (m_meta.empty()) {
        if (row != 0) {
            return;
        }
    } else if (row < 0 || row > int(m_meta.size())) {
        qDebug() << " bad row";
        return;
    }
    
    m_meta.insert(m_meta.begin() + row, nmeta.begin(), nmeta.end());

    // Find the playing track (could be part of nmeta if this is a d&d
    // cut/paste) and adjust m_play_idx
    m_play_idx = -1;
    for (auto it = m_meta.begin(); it != m_meta.end(); it++) {
        if (it->pl_playing && m_play_idx == -1) {
            m_play_idx = it - m_meta.begin();
        } else {
            it->pl_playing = false;
        }
    }

    // Prepare following track
    send_next_playing_signal();

    emit sig_playlist_updated(m_meta, m_play_idx, 0);
}
