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

#include "playlistohrd.h"

#include "libupnpp/log.hxx"

using namespace UPnPP;

    // We take ownership of the OHPlayer object
PlaylistOHRD::PlaylistOHRD(OHRad *ohrd, QObject * parent)
    : Playlist(parent), m_ohrdo(ohrd)
{
    // Connections from OpenHome renderer to local playlist
    connect(m_ohrdo, SIGNAL(metadataArrayChanged(const MetaDataList&)),
            this, SLOT(psl_new_ohrd(const MetaDataList&)));
    connect(m_ohrdo, SIGNAL(currentTrackId(int)),
            this, SLOT(psl_currentTrackId(int)));
    connect(m_ohrdo, SIGNAL(audioStateChanged(int, const char *)),
            this, SLOT(psl_new_transport_state(int, const char *)));

    // Connections from local playlist to openhome
    connect(this, SIGNAL(sig_row_activated(int)),
            m_ohrdo, SLOT(setIdx(int)));
    connect(this, SIGNAL(sig_pause()), m_ohrdo, SLOT(pause()));
    connect(this, SIGNAL(sig_stop()),  m_ohrdo, SLOT(stop()));
    connect(this, SIGNAL(sig_resume_play()), m_ohrdo, SLOT(play()));
}

static bool samelist(const MetaDataList& mdv1, const MetaDataList& mdv2)
{
    if (mdv1.size() != mdv2.size())
        return false;
    for (unsigned int i = 0; i < mdv1.size(); i++) {
        if (mdv1[i].filepath.compare(mdv2[i].filepath))
            return false;
    }
    return true;
}

void PlaylistOHRD::psl_new_ohrd(const MetaDataList& mdv)
{
    qDebug() << "PlaylistOHRD::psl_new_ohrd: " << mdv.size() << " entries";
    if (!samelist(mdv, m_meta)) {
        m_meta = mdv;
        emit sig_playlist_updated(m_meta, m_play_idx, 0);
    }
}

void PlaylistOHRD::psl_currentTrackId(int id)
{
    qDebug() << "PlaylistOHRD::psl_currentTrackId: " << id;

    if (id <= 0) {
        return;
    } 

    for (vector<MetaData>::iterator it = m_meta.begin(); 
         it != m_meta.end(); it++) {
        if (it->id == id) {
            m_play_idx = it - m_meta.begin();
            // Emit the current index in any case to let the playlist
            // UI scroll to show the currently playing track (some
            // time after a user interaction scrolled it off)
            emit sig_playing_track_changed(m_play_idx);
            // If the track id change was caused by the currently
            // playing track having been removed, the play index did
            // not change but the metadata did, so emit metadata in
            // all cases.
            emit sig_track_metadata(*it);
            return;
        }
    }
    LOGINF("PlaylistOHRD::psl_currentTrackId: track not found in array" << endl);
}

void PlaylistOHRD::psl_play() 
{
    if (m_tpstate ==  AUDIO_STOPPED && valid_row(m_selection_min_row)) {
        emit sig_row_activated(m_selection_min_row);
    } else {
        emit sig_resume_play();
    }
}

void PlaylistOHRD::psl_pause() 
{
    emit sig_pause();
}

void PlaylistOHRD::psl_stop() 
{
    emit sig_stop();
}
