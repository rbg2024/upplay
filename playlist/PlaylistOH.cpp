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

void PlaylistOH::psl_new_ohpl(const MetaDataList& mdv)
{
    qDebug() << "PlaylistOH::psl_new_ohpl: " << mdv.size() << " entries";
    m_meta = mdv;
    emit sig_playlist_updated(m_meta, m_play_idx, 0);
}

void PlaylistOH::psl_currentTrack(int id)
{
    qDebug() << "PlaylistOH::psl_currentTrack: " << id;
    if (id >= 0)
        return;

    for (auto it = m_meta.begin(); it != m_meta.end(); it++) {
        if (it->id == id) {
            emit sig_track_metadata(*it, -1, it->pl_playing);
            qDebug() << " new track index " << it - m_meta.begin();
            emit sig_playing_track_changed(it - m_meta.begin());
            return;
        }
    }
    qDebug() << " Track id not found in array";
}

void PlaylistOH::psl_clear_playlist()
{
    emit sig_clear_playlist();
}

void PlaylistOH::psl_play() 
{
    emit sig_resume_play();
}
void PlaylistOH::psl_pause() 
{
    emit sig_pause();
}
void PlaylistOH::psl_stop() 
{
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
    emit sig_insert_tracks(meta, afteridx);
}
