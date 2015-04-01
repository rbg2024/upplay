/* Copyright (C) 2014 J.F.Dockes
 *	 This program is free software; you can redistribute it and/or modify
 *	 it under the terms of the GNU General Public License as published by
 *	 the Free Software Foundation; either version 2 of the License, or
 *	 (at your option) any later version.
 *
 *	 This program is distributed in the hope that it will be useful,
 *	 but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	 GNU General Public License for more details.
 *
 *	 You should have received a copy of the GNU General Public License
 *	 along with this program; if not, write to the
 *	 Free Software Foundation, Inc.,
 *	 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <QDebug>

#include "PlaylistOH.h"

#include "libupnpp/log.hxx"

using namespace UPnPP;


void PlaylistOH::psl_new_ohpl(const MetaDataList& mdv)
{
    qDebug() << "PlaylistOH::psl_new_ohpl: " << mdv.size() << " entries";
    m_meta = mdv;
    emit sig_playlist_updated(m_meta, m_play_idx, 0);
}

void PlaylistOH::psl_trackIdChanged(int id)
{
    //qDebug() << "PlaylistOH::psl_trackIdChanged: " << id;
    if (id <= 0)
        return;

    for (vector<MetaData>::iterator it = m_meta.begin(); 
         it != m_meta.end(); it++) {
        if (it->id == id) {
            if (m_play_idx != it - m_meta.begin()) {
                emit sig_playing_track_changed(it - m_meta.begin());
                m_play_idx = it - m_meta.begin();
                //qDebug() << " new track index " << m_play_idx;
            }
            // If the track id change was caused by the currently
            // playing track having been removed, the play index did
            // not change but the metadata did, so emit metadata in
            // all cases.
            emit sig_track_metadata(*it);
            return;
        }
    }
    LOGINF("PlaylistOH::psl_trackIdChanged: track not found in array" << endl);
}

void PlaylistOH::psl_clear_playlist_impl()
{
    // Tell the Open Home Playlist to do it.
    emit sig_clear_playlist();
}

void PlaylistOH::psl_play() 
{
    if (m_tpstate ==  AUDIO_STOPPED && valid_row(m_selection_min_row)) {
        emit sig_row_activated(m_selection_min_row);
    } else {
        emit sig_resume_play();
    }
    m_pause = false;
}

void PlaylistOH::psl_pause() 
{
    m_pause = true;
    emit sig_pause();
}

void PlaylistOH::psl_stop() 
{
    m_pause = true;
    emit sig_stop();
}

void PlaylistOH::psl_forward() 
{
    emit sig_forward();
}

void PlaylistOH::psl_backward() 
{
    emit sig_backward();
}

void PlaylistOH::psl_insert_tracks(const MetaDataList& meta, int afteridx)
{
    qDebug() << "PlaylistOH::psl_insert_tracks ntracks " << meta.size() << 
        " afteridx" << afteridx;
    emit sig_insert_tracks(meta, afteridx);
}

void PlaylistOH::psl_remove_rows(const QList<int>& rows, bool)
{
    if (rows.contains(m_play_idx)) {
        m_play_idx = -1;
    }
    emit sig_tracks_removed(rows);
}
