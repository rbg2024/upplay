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

#include <QList>
#include <QDebug>


#include "Playlist.h"

Playlist::Playlist(QObject* parent) 
    : QObject (parent), m_play_idx(-1), m_selection_min_row(-1),
      m_insertion_point(-1), m_tpstate(AUDIO_UNKNOWN), m_pause(false)

{
    m_play_idx = -1;
    m_selection_min_row = -1;
    m_tpstate = AUDIO_UNKNOWN;
    m_pause = false;
}

// GUI -->
void Playlist::psl_clear_playlist()
{
    m_meta.clear();
    m_play_idx = -1;
    m_selection_min_row = -1;
    m_insertion_point = -1;
    psl_clear_playlist_impl();
}

// Remove one row
void Playlist::remove_row(int row)
{
    //qDebug() << "Playlist::remove_row";
    QList<int> remove_list;
    remove_list << row;
    psl_remove_rows(remove_list);
}

void Playlist::psl_new_transport_state(int tps, const char *)
{
//    qDebug() << "Playlist::psl_new_transport_state " << s <<
//        " play_idx " << m_play_idx;
//    if (m_play_idx >= 0 && m_play_idx < int(m_meta.size())) 
//        qDebug() << "     meta[idx].pl_playing " << 
//            m_meta[m_play_idx].pl_playing;

    m_tpstate = tps;
    switch (tps) {
    case AUDIO_UNKNOWN:
    case AUDIO_STOPPED:
    default:
        emit sig_stopped();
        break;
    case AUDIO_PLAYING:
        emit sig_playing();
        break;
    case AUDIO_PAUSED:
        emit sig_paused();
        break;
    }
}

// GUI -->
void Playlist::psl_change_mode(const Playlist_Mode& mode)
{
    CSettingsStorage::getInstance()->setPlaylistMode(mode);
    emit sig_mode_changed(mode);
}

void Playlist::psl_add_tracks(const MetaDataList& v_md)
{
    qDebug() << "Playlist::psl_add_tracks: " <<
        " ninserttracks " <<  v_md.size() << 
        " m_meta.size() " << m_meta.size();

    emit sig_sync();

    Playlist_Mode playlist_mode = CSettingsStorage::getInstance()->getPlaylistMode();
    if (playlist_mode.replace)
        psl_clear_playlist();

    int afteridx = -1;

    if (playlist_mode.append) {
        afteridx = m_meta.size() - 1;
    } else {
        if (m_insertion_point >= 0) {
            afteridx = m_insertion_point;
        } else if (m_play_idx >= 0) {
            afteridx = m_play_idx;
            // Using selection_min_row appears to be source of confusion
            // } else if (m_selection_min_row >= 0) { 
            // afteridx = m_selection_min_row;
        } else {
            // The more natural thing to do if neither playing nore
            // selection is to append. Else clicking on several tracks
            // inserts them in reverse order
            afteridx = m_meta.size() - 1;
        }
    }

    psl_insert_tracks(v_md, afteridx);
    m_insertion_point = afteridx + 1;

    if (playlist_mode.playAdded) {
        psl_change_track(afteridx+1);
        psl_play();
    }

    emit insertDone();
}

void Playlist::psl_selection_min_row(int row)
{
    qDebug() << "Playlist::psl_selection_min_row: " << row;
    m_selection_min_row = row;
}
